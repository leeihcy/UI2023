#include "windowrender.h"
#include "compositor.h"
#include "include/inc.h"
#include "include/interface/renderlibrary.h"
#include "include/util/log.h"
#include "include/util/rect_region.h"
#include "src/application/config/config.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/layer/hardware_compositor.h"
#include "src/layer/software_compositor.h"
#include "src/object/object.h"
#include "src/panel/root_object.h"
#include "src/window/window.h"

namespace ui {
WindowRender::WindowRender(Window &w) : m_window(w) {}

WindowRender::~WindowRender() {}

IWindowRender *WindowRender::GetIWindowRender() {
  if (!m_pIWindowRender)
    m_pIWindowRender.reset(new IWindowRender(this));

  return m_pIWindowRender.get();
}

void WindowRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "WindowRender");

  s.AddEnum(XML_WINDOW_GRAPHICS_RENDER_LIBRARY, *(int *)&m_grl_type)
      ->FillGraphicsRenderLibraryData()
      ->SetDefault(GRAPHICS_RENDER_LIBRARY_TYPE_SKIA);

  s.AddBool(XML_WINDOW_NEED_ALPHACHANNEL, m_need_alpha_channel)
      ->SetDefault(true);
}

void WindowRender::AddInvalidateRect(const Rect *dirty) {
  // 不接受空的dirty，调用者必须明确指定无效区域，
  // 以减少全局刷新效率低的问题。
  if (!dirty) {
    return;
  }
  if (dirty->IsEmpty()) {
    return;
  }

  if (Config::GetInstance().debug.log_window_onpaint) {
    UI_LOG_DEBUG("WindowRender::AddInvalidateRect: (%d,%d)-(%d,%d)",
                 dirty->left, dirty->top, dirty->right, dirty->bottom);
  }

  Layer *layer = m_window.GetRootObject().GetLayer();
  if (!layer) {
    return;
  }
  layer->Invalidate(dirty);
}

// IRenderTarget* 没有引用计数机制
// 但仍然采用Release进行释放（delete）
bool WindowRender::CreateRenderTarget(IRenderTarget **pp) {
  if (!pp)
    return false;

  auto *app = m_window.GetResource().GetUIApplication();
  *pp = UICreateRenderTarget(app->GetIUIApplication(), m_grl_type,
                             m_need_alpha_channel);
  return true;
}

// 逻辑单位
void WindowRender::OnClientSize(unsigned int nWidth, unsigned int nHeight) {
  if (m_compositor) {
    m_compositor->Resize(nWidth, nHeight);
  }
}

void WindowRender::SetCanCommit(bool b) {
  if (b)
    m_can_commit--;
  else
    m_can_commit++;
}

bool WindowRender::CanCommit() { return 0 == m_can_commit; }

void WindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE type) {
  // 仅在窗口创建之前设置有用
  m_grl_type = type;
}
GRAPHICS_RENDER_LIBRARY_TYPE WindowRender::GetGraphicsRenderType() {
  return m_grl_type;
}
bool WindowRender::GetRequireAlphaChannel() { return m_need_alpha_channel; }

Layer *WindowRender::CreateLayer(IObjectLayerContent *pContent) {
  if (!pContent)
    return nullptr;

  Object &obj = pContent->GetObj();
  Layer *pNewLayer = get_create_compositor()->CreateLayer();
  pNewLayer->SetContent(pContent);

  if (!m_compositor->GetRootLayer()) {
    m_compositor->SetRootLayer(pNewLayer);
    return pNewLayer;
  }

  // 获取父layer
  Layer *pParentLayer = pContent->GetParentLayer();
  if (!pParentLayer) {
    // 有根结点，但没父结点？
    UIASSERT(0);
    pParentLayer = m_compositor->GetRootLayer();
  }

  // 计算这个layer应该插入在layer tree的哪个位置，需要与object tree对应
  Layer *pNextLayer = pContent->GetNextLayer();
  pParentLayer->AddSubLayer(pNewLayer, pNextLayer);

  // 整理子对象的layer
  Layer *p = pParentLayer->GetFirstChild();
  while (p) {
    if (p != pNewLayer) {
      ILayerContent *pLayerContent = p->GetContent();

      // 发现自己的子对象，替换它，并将它作为自己的子layer
      if (pLayerContent && pLayerContent->IsChildOf(pContent)) {
        p->MoveLayer2NewParentEnd(pParentLayer, pNewLayer);
      }
    }
    p = p->GetNext();
  }

  return pNewLayer;
}

Layer *WindowRender::CreateLayer(IListItemLayerContent *pContent) {
  if (!pContent)
    return nullptr;

  ListItemBase &item = pContent->GetListItem();
  Layer *pNewLayer = get_create_compositor()->CreateLayer();
  pNewLayer->SetContent(pContent);

  if (!m_compositor->GetRootLayer()) {
    m_compositor->SetRootLayer(pNewLayer);
    return pNewLayer;
  }

  // 获取父layer
  Layer *pParentLayer = pContent->GetParentLayer();
  if (!pParentLayer) {
    // 有根结点，但没父结点？
    UIASSERT(0);
    pParentLayer = m_compositor->GetRootLayer();
  }

  // 计算这个layer应该插入在layer tree的哪个位置，需要与object tree对应
  Layer *pNextLayer = pContent->GetNextLayer();
  pParentLayer->AddSubLayer(pNewLayer, pNextLayer);

  // 整理子对象的layer
  Layer *p = pParentLayer->GetFirstChild();
  while (p) {
    if (p != pNewLayer) {
      ILayerContent *pLayerContent = p->GetContent();

      // 发现自己的子对象，替换它，并将它作为自己的子layer
      if (pLayerContent && pLayerContent->IsChildOf(pContent)) {
        p->MoveLayer2NewParentEnd(pParentLayer, pNewLayer);
      }
    }
    p = p->GetNext();
  }

  return pNewLayer;
}

Compositor *WindowRender::get_create_compositor() {
  if (!m_compositor) {
    if (m_window.IsGpuComposite()) {
      m_compositor.reset(new HardwareCompositor());
    } else {
      m_compositor.reset(new SoftwareCompositor());
    }
    auto *app = m_window.GetResource().GetUIApplication();
    m_compositor->SetUIApplication(app);
    m_compositor->SetWindowRender(this);

    m_compositor->BindWindow(&m_window);
  }

  return m_compositor.get();
}

void WindowRender::onWindowCreated() {
  if (Config::GetInstance().enable_render_thread) {
    // 标记root layer对应的rendertarget需要创建双缓存用于提交数据。
    // 其它layer的rendertarget不需要创建双缓存。

    auto *root_layer = m_window.GetRootObject().GetSelfLayer();
    assert(root_layer);
    assert(root_layer->GetRenderTarget());
    root_layer->GetRenderTarget()->CreateSwapChain(m_window.IsGpuComposite());
  }

  // 首次刷新，将窗口所有区域设置为无效
  ui::Rect rc_client;
  m_window.GetRootObject().GetClientRectWithZeroOffset(&rc_client);
  AddInvalidateRect(&rc_client);
}

void WindowRender::RequestInvalidate() {
  if (m_request_invalidate_ref == 0) {
    weak_ptr<WindowRender> ptr = m_weakptr_factory.get();
    auto *app = m_window.GetResource().GetUIApplication();

    app->GetMessageLoop().PostTask(Slot(&WindowRender::InvalidateNow, ptr));
    m_request_invalidate_ref++;
  }
}

void WindowRender::InvalidateNow() { Paint(nullptr); }

void WindowRender::Paint(const Rect *commit_rect) {
  m_request_invalidate_ref = 0;

  if (Config::GetInstance().debug.log_window_onpaint && commit_rect) {
    UI_LOG_DEBUG("Window::onPaint commit_rect={%d,%d, %d,%d}",
                 (int)(commit_rect->left), (int)(commit_rect->top),
                 (int)(commit_rect->right), (int)(commit_rect->bottom));
  }

  if (!CanCommit()) {
    return;
  }
  if (!m_compositor) {
    return;
  }

  RectRegion dirty_region;
  if (m_compositor->UpdateDirty(&dirty_region)) {
    // 没有脏区域时，不需要swap chain

    auto *root_layer = m_window.GetRootObject().GetSelfLayer();
    assert(root_layer);
    assert(root_layer->GetRenderTarget());
    root_layer->GetRenderTarget()->SwapChain(Slot(
        &WindowRender::on_swap_chain, m_weakptr_factory.get(), dirty_region));
  }

  // 窗口额外无效的区域，但不是我们的脏区域。只需要直接commit上去即可。
  if (commit_rect) {
    dirty_region.Union(*commit_rect);
  }
  m_compositor->Commit(dirty_region);
}

void WindowRender::on_swap_chain(DirtyRegion dirty_region) {
  DirectCommit(dirty_region);
}

// 在swap chain之后，直接提交新的内容到窗口上
void WindowRender::DirectCommit(const DirtyRegion &dirty_region) {
  if (!CanCommit()) {
    return;
  }
  if (!m_compositor) {
    return;
  }
  m_compositor->Commit(dirty_region);
}

void WindowRender::SoftwareCommit(IRenderTarget *pRT,
                                  const RectRegion &dirtyInWindow) {
  FrameBufferWithReadLock frame_buffer = {0};
    if (!pRT->GetFrontFrameBuffer(&frame_buffer)) {
      return;
    }
  m_window.m_platform->Commit2(frame_buffer, dirtyInWindow);
}

} // namespace ui