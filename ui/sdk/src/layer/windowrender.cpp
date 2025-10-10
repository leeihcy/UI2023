#include "windowrender.h"
#include "gpu/include/api.h"
#include "include/common/signalslot/slot.h"
#include "include/interface/renderlibrary.h"
#include "include/macro/helper.h"
#include "include/util/log.h"
#include "include/util/rect_region.h"
#include "layer_rt.h"
#include "layer_sync_op.h"
#include "src/application/config/config.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/object/object.h"
#include "src/panel/root_object.h"
#include "src/window/window.h"
#include <cstddef>
#include <memory>

namespace ui {
WindowRender::WindowRender(Window &w)
    : m_window(w), m_rt(std::make_shared<WindowRenderRT>(*this)) {
  RenderThread::Main::PostTask(ui::Slot(&WindowRenderRTMap::Bind,
                                        &WindowRenderRTMap::GetInstance(),
                                        (void *)m_rt.get(), m_rt));
}

WindowRender::~WindowRender() {
  RenderThread::Main::PostTask(ui::Slot(&WindowRenderRTMap::Unbind,
                                        &WindowRenderRTMap::GetInstance(),
                                        (void *)m_rt.get()));
}

IWindowRender *WindowRender::GetIWindowRender() {
  if (!m_pIWindowRender)
    m_pIWindowRender.reset(new IWindowRender(this));

  return m_pIWindowRender.get();
}

Application& WindowRender::GetUIApplication() {
  return m_window.GetApplication();
}
void WindowRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "WindowRender");

  s.AddEnum(XML_WINDOW_GRAPHICS_RENDER_LIBRARY, *(int *)&m_grl_type)
      ->FillGraphicsRenderLibraryData()
      ->SetDefault(GRAPHICS_RENDER_LIBRARY_TYPE_SKIA);

  s.AddBool(XML_WINDOW_NEED_ALPHACHANNEL, m_need_alpha_channel)
      ->SetDefault(true);
}

bool WindowRender::IsHardwareComposite() { 
  return m_window.IsHardwareComposite(); 
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
  if (IsHardwareComposite()) {
    // 像素大小
    ui::Rect rc;
    m_window.GetWindowPlatform()->GetClientRect(&rc);

    RenderThread::Main::PostTask(ui::Slot(
        &WindowRenderRT::Resize, m_rt->m_factory.get(),
        rc.Width(), rc.Height()));
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

Layer* WindowRender::GetRootLayer() {
  return m_window.GetRootObject().GetLayer();
}

Layer *WindowRender::CreateLayer(IObjectLayerContent *pContent) {
  if (!pContent)
    return nullptr;

  Object &obj = pContent->GetObj();
  Layer *pNewLayer = nullptr;
  if (IsHardwareComposite()) {
    pNewLayer = new Layer(Layer_Hardware);
  } else {
    pNewLayer = new Layer(Layer_Software);
  }
  pNewLayer->SetWindowRender(this);
  pNewLayer->SetContent(pContent);

  // 获取父layer
  Layer *pParentLayer = pContent->GetParentLayer();
  if (!pParentLayer) {
    pParentLayer = GetRootLayer();
  }
  if (!pParentLayer) {
    return pNewLayer;
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
  Layer *pNewLayer = nullptr;
  if (IsHardwareComposite()) {
    pNewLayer = new Layer(Layer_Hardware);
  } else {
    pNewLayer = new Layer(Layer_Software);
  }
  pNewLayer->SetWindowRender(this);
  pNewLayer->SetContent(pContent);

  // 获取父layer
  Layer *pParentLayer = pContent->GetParentLayer();
  if (!pParentLayer) {
    pParentLayer = GetRootLayer();
  }
  if (!pParentLayer) {
    return pNewLayer;
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

void WindowRender::onWindowCreated() {
  if (IsHardwareComposite()) {
    IGpuCompositorWindow *p = m_window.GetWindowPlatform()->GetGpuCompositorWindow();

    // 像素大小
    ui::Rect rc;
    m_window.GetWindowPlatform()->GetClientRect(&rc);
    RenderThread::Main::PostTask(
        ui::Slot(&WindowRenderRT::CreateHardwareCompositor,
                 m_rt->m_factory.get(), p, rc));
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

void WindowRender::Paint(const Rect *commit_rect_px) {
  m_request_invalidate_ref = 0;

  if (Config::GetInstance().debug.log_window_onpaint && commit_rect_px) {
    UI_LOG_DEBUG("Window::onPaint commit_rect={%d,%d, %d,%d}",
                 (int)(commit_rect_px->left), (int)(commit_rect_px->top),
                 (int)(commit_rect_px->right), (int)(commit_rect_px->bottom));
  }

  if (!CanCommit()) {
    return;
  }

  RectRegion dirty_region;
  if (update_dirty(&dirty_region)) {
    // 没有脏区域时，不需要swap chain

    auto *root_layer = m_window.GetRootObject().GetSelfLayer();
    assert(root_layer);
    assert(root_layer->GetRenderTarget());
    root_layer->GetRenderTarget()->SwapChain(Slot(
        &WindowRender::on_swap_chain, m_weakptr_factory.get(), dirty_region));
  }

  RectRegion dirty_region_px;
  for (unsigned int i = 0; i < dirty_region.Count(); i++) {
    ui::Rect rc = dirty_region.RectPtr2()[i];
    m_window.m_dpi.ScaleRect(&rc);
    dirty_region_px.AddRect(rc);
  }
  // 窗口额外无效的区域，但不是我们的脏区域。只需要直接commit上去即可。
  if (commit_rect_px) {
    dirty_region_px.Union(*commit_rect_px);
  }
  commit(dirty_region_px);
}


bool WindowRender::update_dirty(RectRegion *outDirtyInWindow) {
  Layer* root_layer = GetRootLayer();
  if (!root_layer)
    return false;

  if (IsHardwareComposite()) {
    // 硬件合成只能是每个层分别去调用updatedirty，而不是像软件渲染一样由parent
    // object遍历child时去调用
    // updatedirty。因为硬件下父layer可能没有dirty，而子layer有dirty.
    Layer *p = root_layer;
    // p->UpdateDirty();

    while (p) {
      hardware_update_dirty_recursion(p);
      p = p->GetNext();
    }
    return true;
  } else {
    // 软件渲染需要返回脏区域的窗口坐标，用于增量提交到最终窗口上面

    // 先返回当前窗口脏区域
    root_layer->CopyDirtyRect(outDirtyInWindow);

    // 更新各个层
    return root_layer->UpdateDirty();
  }
  return false;
}

void WindowRender::hardware_update_dirty_recursion(Layer *p) {
  UIASSERT(p);
  p->UpdateDirty();

  p = p->GetFirstChild();
  while (p) {
    hardware_update_dirty_recursion(p);
    p = p->GetNext();
  }
}

void WindowRender::commit(const RectRegion & dirty_region_px) {
  Layer *root_layer = GetRootLayer();
  if (!root_layer)
    return;

  if (IsHardwareComposite()) {
    RenderThread::Main::PostTask(
        ui::Slot(&WindowRenderRT::HardwareCommit, m_rt->m_factory.get()));
  } else {
#if 0
    // 给分层窗口提交的机会。
    if (m_window_render->m_window.virtualCommitReq())
      return;
#endif

    SoftwareCommit(root_layer->GetRenderTarget(), dirty_region_px);
  }
}

void WindowRender::on_swap_chain(DirtyRegion dirty_region_dip) {
  DirectCommit(dirty_region_dip);
}

// 在swap chain之后，直接提交新的内容到窗口上
void WindowRender::DirectCommit(DirtyRegion dirty_region_dip) {
  if (!CanCommit()) {
    return;
  }

  DirtyRegion& dirty_region_px = dirty_region_dip;
  for (unsigned int i = 0; i < dirty_region_px.Count(); i++) {
    Rect* rect = dirty_region_px.GetRectPtrAt(i);
    m_window.m_dpi.ScaleRect(rect);
  }
  
  commit(dirty_region_px);
}

void WindowRender::SoftwareCommit(IRenderTarget *pRT,
                                  const RectRegion &dirty_region_px) {
  FrameBufferWithReadLock frame_buffer;
  if (!pRT->GetFrontFrameBuffer(&frame_buffer)) {
    return;
  }
  m_window.m_platform->Commit2(frame_buffer, dirty_region_px);
}

WindowRenderRT::WindowRenderRT(WindowRender& w) : m_window_render(w) {}

WindowRenderRT::~WindowRenderRT() {
  m_layer_map.clear();
  if (m_root_layer) {
    m_root_layer.reset();
  }
  if (m_gpu_composition) {
    m_gpu_composition.reset();
  }
}

void WindowRenderRT::CreateHardwareCompositor(IGpuCompositorWindow *window,
                                              ui::Rect rc) {
  m_gpu_composition = ui::CreateGpuComposition(window);
  if (!m_gpu_composition) {
    return;
  }
  m_gpu_composition->Resize(rc.Width(), rc.Height());
}

void WindowRenderRT::Resize(uint width, uint height) {
  if (!m_gpu_composition) {
    return;
  }
  m_gpu_composition->Resize(width, height);
}

void WindowRenderRT::HardwareCommit() {
  if (!m_gpu_composition)
    return;

  GpuLayerCommitContext context;
  if (!m_gpu_composition->BeginCommit(&context))
    return;

  LayerRT *p = m_root_layer.get();
  while (p) {
    hardwareCommit2(p, &context);
    p = p->m_pNext;
  }

  m_gpu_composition->EndCommit(&context);
}

void WindowRenderRT::hardwareCommit2(LayerRT *p,
                                     GpuLayerCommitContext *pContext) {

  if (!p->m_properties.visible)
    return;

  GpuLayerCommitContext context(*pContext);
  p->HardwareCommit(&context);

  p = p->m_pFirstChild;
  while (p) {
    hardwareCommit2(p, &context);
    p = p->m_pNext;
  }
}

std::shared_ptr<LayerRT> WindowRenderRT::find_layer(LAYERID layer_id) {
  if (layer_id == 0) {
    return nullptr;
  }
  auto iter = m_layer_map.find(layer_id);
  if (iter == m_layer_map.end()) {
    return nullptr;
  }
  return iter->second;
}

void WindowRenderRT::SyncLayerProperties(LAYERID layer_id, LayerTreeProperties properties) {
  std::shared_ptr<LayerRT> layer = find_layer(layer_id);
  if (!layer) {
    return;
  }
  layer->m_properties = properties;
}

void WindowRenderRT::OnLayerTreeChanged(LayerTreeSyncOperation op) {
  if (op.type == LayerTreeSyncOpType::Add) {
    std::shared_ptr<LayerRT> layer = find_layer(op.id);
    if (!layer) {
      auto new_layer = std::make_shared<LayerRT>(op.id);
      m_layer_map[op.id] = new_layer;
      layer = new_layer;
    }

    std::shared_ptr<LayerRT> parent = find_layer(op.parent_id);
    std::shared_ptr<LayerRT> next = find_layer(op.next_id);
    if (!parent) {
      m_root_layer = layer;
    } else {
      parent->AddSubLayer(layer.get(), next.get());
    }
  } 
  else if (op.type == LayerTreeSyncOpType::Delete) {
    m_layer_map.erase(op.id);
    if (m_root_layer && m_root_layer->m_layer_id == op.id) {
      m_root_layer = nullptr;
    }
  }
  else if (op.type == LayerTreeSyncOpType::Remove) {
    std::shared_ptr<LayerRT> layer = find_layer(op.id);
    if (layer) {
      layer->RemoveMeInTheTree();
    }
  } else {
    UIASSERT(false && "TODO:");
  }
}

void WindowRenderRT::BindLayer(LAYERID layer_id, std::shared_ptr<IGpuLayer> gpu_layer) {
  std::shared_ptr<LayerRT> layer = find_layer(layer_id);
  if (!layer) {
    auto new_layer = std::make_shared<LayerRT>(layer_id);
    m_layer_map[layer_id] = new_layer;
    layer = new_layer;
  }
  layer->BindGpuLayer(gpu_layer);

  // TBD: 将第一个layer作为root layer?
  if (!m_root_layer) {
    m_root_layer = layer;
  }
}

//-------

// static 
WindowRenderRTMap &WindowRenderRTMap::GetInstance() {
  static WindowRenderRTMap s;
  return s;
}
void WindowRenderRTMap::Bind(void *key, std::shared_ptr<WindowRenderRT> rt) {
  m_window_render_map[key] = rt;
}
void WindowRenderRTMap::Unbind(void *key) { m_window_render_map.erase(key); }
std::shared_ptr<WindowRenderRT> WindowRenderRTMap::find(void *key) {
  auto iter = m_window_render_map.find(key);
  if (iter != m_window_render_map.end()) {
    return iter->second;
  }
  return std::shared_ptr<WindowRenderRT>();
}

} // namespace ui