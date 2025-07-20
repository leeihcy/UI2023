#include "windowrender.h"
#include "compositor.h"
#include "include/inc.h"
#include "include/util/log.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/object/object.h"
#include "src/panel/root_object.h"
#include "src/window/window.h"
#include "src/application/uiapplication.h"
// #include "include/interface/iwndtransmode.h"
// #include "include/interface/irenderlayer.h"
#include "src/layer/hardware_compositor.h"
#include "src/layer/software_compositor.h"
// #include "..\Util\Stopwatch\stopwatch.h"
// #include "..\UIObject\ListCtrl\ListItemBase\listitembase.h"

namespace ui {
WindowRender::WindowRender(Window &w) : m_window(w) {
  //	m_pHardwareComposition = nullptr;
  //  m_bNeedRebuildGpuLayerTree = true;
}

WindowRender::~WindowRender() {
  //	SAFE_RELEASE(m_pHardwareComposition);
}

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

  UI_LOG_INFO("WindowRender::AddInvalidateRect: (%d,%d)-(%d,%d)", 
    dirty->left, dirty->top, dirty->right, dirty->bottom);

  Layer *layer = m_window.GetRootObject().GetLayer();
  if (!layer) {
    return;
  }
  layer->Invalidate(dirty);
}

void WindowRender::Paint(const Rect *commit_rect) {
  if (!CanCommit()) {
    UI_LOG_WARN(L"can not commit now");
    return;
  }
  if (!m_compositor) {
    return;
  }

  RectRegion arrDirtyInWindow;
  m_compositor->UpdateDirty(&arrDirtyInWindow);
  
  // if (commit_rect) {
  //   arrDirtyInWindow.Union(*commit_rect);
  // }
  m_compositor->Commit(arrDirtyInWindow);
}

void WindowRender::RequestUpdate() {

}

// IRenderTarget* 没有引用计数机制
// 但仍然采用Release进行释放（delete）
bool WindowRender::CreateRenderTarget(IRenderTarget **pp) {
  if (!pp)
    return false;

  auto *app = m_window.GetResource().GetUIApplication();
  *pp = UICreateRenderTarget(app->GetIUIApplication(), m_grl_type, m_need_alpha_channel);
  return true;
}

void WindowRender::OnWindowSize(unsigned int nWidth, unsigned int nHeight) {
  if (m_compositor)
    m_compositor->Resize(nWidth, nHeight);
}
void WindowRender::OnWindowPaint(const Rect &dirty) {
  // 由窗口自动触发的更新，只要直接提交缓存到窗口即可。
  // 但如果有脏区域，还是要处理一下。

  if (!CanCommit()) {
    UI_LOG_WARN(L"can not commit now");
    return;
  }
  if (!m_compositor) {
    return;
  }

  RectRegion arrDirtyInWindow;
  m_compositor->UpdateDirty(&arrDirtyInWindow);

  arrDirtyInWindow.Union(dirty);
  m_compositor->Commit(arrDirtyInWindow);
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

void WindowRender::UpdateAndCommit() {
  if (!m_compositor)
    return;

  m_compositor->UpdateAndCommit();
}

void WindowRender::OnWindowCreate() {
  if (!m_compositor) {
    return;
  }
  m_compositor->BindWindow(&m_window);
}

// void  WindowRender::UpdateWindow(HDC hDC, Rect* prcDamageArray, uint nCount)
// {
//     Rect rcClient;
//     if (!prcDamageArray)
//     {
// 		::GetClientRect(m_pWindow->m_hWnd, &rcClient);
// 		prcDamageArray = &rcClient;
// 		nCount = 1;
// 	}
//
// 	RenderLayer* pRenderLayer = m_pWindow->GetRenderLayer2();
// 	if (!pRenderLayer)
// 		return;
//
// 	pRenderLayer->UpdateLayer(prcDamageArray, nCount);
// 	Commit(hDC, prcDamageArray, nCount);
// }

// IGpuLayer*  WindowRender::CreateGpuLayerTexture(RenderLayer* p)
// {
// 	if (nullptr == m_pHardwareComposition)
// 	{
// 		if (m_pWindow->IsGpuComposite())
// 		{
// 			m_pHardwareComposition =
// CreateHardwareComposition(m_pWindow->m_hWnd);
// 		}
// 	}
//
// 	IGpuLayer* pGpuTexture = nullptr;
// 	if (m_pHardwareComposition)
// 	{
// 		pGpuTexture = m_pHardwareComposition->CreateLayerTexture();
// 		if (p->GetCreateObject() == m_pWindow)
// 			m_pHardwareComposition->SetRootLayerSurface(pGpuTexture);
//
// 		m_bNeedRebuildGpuLayerTree = true;
// 	}
//
// 	return pGpuTexture;
// }
//
// void  WindowRender::OnRenderLayerDestroy(RenderLayer* p)
// {
//     if (m_pHardwareComposition)
//     {
//         m_bNeedRebuildGpuLayerTree = true;
//     }
// }
// void  WindowRender::DrawIncrement()
// {
//     m_compositor->UpdateDirty(nullptr);
//
// }
//
// void  WindowRender::Commit(HDC hDC, Rect* prc, int nCount)
// {
//     if (!CanCommit())
//         return;
//
//     if (m_pHardwareComposition)
//     {
//         HardComposite();
//     }
//     else
//     {
//         m_pWindow->CommitDoubleBuffet2Window(hDC, prc, prc?nCount:0);
//     }
// }
//
// void  WindowRender::HardComposite()
// {
//     if (m_pHardwareComposition)
//     {
//         if (m_bNeedRebuildGpuLayerTree)
//             this->RebuildCompositingLayerTree();
//
//         if (m_pHardwareComposition->BeginCommit())
//         {
//             RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
//
// 			Rect rcClip;
// 			::GetClientRect(m_pWindow->GetHWND(), &rcClip);
//
// 			GpuLayerCommitContext context;
// 			context.SetClipRect(&rcClip);
//             pRootLayer->Compositor2Gpu(context);
//             m_pHardwareComposition->EndCommit();
//         }
//     }
// }
//
// void  WindowRender::OnHardCompositeChanged(bool bEnable)
// {
// 	if (m_bNeedRebuildGpuLayerTree)
// 		this->RebuildCompositingLayerTree();
//
// 	RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
//     if (pRootLayer)
//     {
// 	    pRootLayer->OnHardCompositeChanged(bEnable);
//     }
// }
//
// void  WindowRender::SetNeedRebuildCompositingLayerTree()
// {
// 	m_bNeedRebuildGpuLayerTree = true;
// }
//
// // 通过renderlayer树来构建gpulayer树
// void  WindowRender::RebuildCompositingLayerTree()
// {
//     if (!m_bNeedRebuildGpuLayerTree)
//         return;
//
//     RenderLayer*  pRootLayer = m_pWindow->GetRenderLayer2();
//     if (!pRootLayer)
//         return;
//
//     pRootLayer->ClearChildren();
//     rebuildCompositingLayerTree(m_pWindow,pRootLayer);
//
//     m_bNeedRebuildGpuLayerTree = false;
// }
// void  WindowRender::rebuildCompositingLayerTree(
//         Object* pParent,
//         RenderLayer* pParentLayer)
// {
//     if (!pParent || !pParentLayer)
//         return;
//
//     Object*  pChild = nullptr;
//     RenderLayer*  pLayer = nullptr;
//
//     while (pChild = pParent->EnumAllChildObject(pChild))
//     {
//         pLayer = pChild->GetSelfRenderLayer2();
//         if (pLayer)
//         {
// //             IGpuLayer*  pGpuLayerTexture = pLayer->GetGpuTexture();
// //             UIASSERT(pGpuLayerTexture);
//
// //             pGpuLayerTexture->ClearChildren();
// //             pParentLayer->AddChild(pGpuLayerTexture);
// // #ifdef _DEBUG
// //             Point pt = pChild->GetWindowPoint();
// //             pGpuLayerTexture->SetWindowPos(pt.x, pt.y);
// // #endif
// //             rebuildCompositingLayerTree(pChild, pGpuLayerTexture);
//
//             pLayer->ClearChildren();
//             pParentLayer->AddChild(pLayer);
//             rebuildCompositingLayerTree(pChild, pLayer);
//         }
//         else
//         {
//             rebuildCompositingLayerTree(pChild, pParentLayer);
//         }
//     }
// }

// void WindowRender::Commit(Rect *prcInvalid) {
//   if (!prcInvalid) {
//     UIASSERT(0);
//     return;
//   }

//   // 如果有脏区域，还是需要先刷新，再提交
//   //
//   例如SIZE变化，进行异步刷新时，重新创建了RenderTarget，然后就来了一个WM_PAINT消息，
//   // 但此时还没有刷新layer，不能直接提交一个空的rendertarget缓存到窗口上！

//   if (m_compositor) {
//     RectRegion arr;

//     Rect r;
//     r.CopyFrom(*prcInvalid);
//     arr.AddRect(r);

//     m_compositor->Commit(arr);
//   }
// }

} // namespace ui