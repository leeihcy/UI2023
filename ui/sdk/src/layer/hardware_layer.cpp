#include "include/macro/helper.h"
#include "hardware_layer.h"
#include "hardware_compositor.h"
#include "include/interface/renderlibrary.h"
#include "ui/UICompositor/Inc/inc.h"

namespace ui {

HardwareLayer::HardwareLayer() { m_pGpuTexture = nullptr; }

HardwareLayer::~HardwareLayer() {
  SAFE_RELEASE(m_pRenderTarget);
  SAFE_RELEASE(m_pGpuTexture);
}

// void  HardwareLayer::DrawFull()
// {
//     if (!m_pLayerContent)
//         return;
//
//     IRenderTarget* pRenderTarget = GetRenderTarget();
//     pRenderTarget->Clear(nullptr);
//
//     pRenderTarget->BeginDraw();
//     m_pLayerContent->Draw(pRenderTarget);
//     pRenderTarget->EndDraw();
//
// 	m_dirtyRectangles.Destroy();
//     upload_2_gpu();
// }

void HardwareLayer::UpdateDirty() {
  if (!m_pLayerContent)
    return;

  if (!m_dirtyRectangles.GetCount())
    return;

  IRenderTarget *pRenderTarget = GetRenderTarget();
  uint nCount = m_dirtyRectangles.GetCount();
  for (uint i = 0; i < nCount; i++)
    pRenderTarget->Clear(m_dirtyRectangles.GetRectPtrAt(i));

  pRenderTarget->BeginDraw();

  pRenderTarget->SetMetaClipRegion(m_dirtyRectangles.GetArrayPtr(),
                                   m_dirtyRectangles.GetCount());

  // 立即销毁无效区域，避免在Draw中再次触发Invalidate逻辑后，dirtyrect又被清空
  // 例如listitem.draw->listitem.delayop->listitem.onsize->invalidate
  m_dirtyRectangles.Destroy();

  m_pLayerContent->Draw(pRenderTarget);
  pRenderTarget->EndDraw();

  upload_2_gpu();
}

// 硬件合成时，每一层的数据是独享的。
// void HardwareLayer::Draw()
// {
// 	this->draw_layer();
//
// 	Layer* pChild = m_pFirstChild;
// 	while (pChild)
// 	{
// 		HardwareLayer* p = static_cast<HardwareLayer*>(pChild);
//
// 		p->draw_layer();
// 		pChild = p->m_pNext;
// 	}
// }
//
//
// void  HardwareLayer::draw_layer()
// {
// 	if (m_dirtyRectangles.GetCount() == 0)
// 		return;
//
// 	// 重新生成layer数据
//
// 	// 提交layer
// 	upload_2_gpu();
//
// 	// 清除脏区域
// 	m_dirtyRectangles.Destroy();
// }

void HardwareLayer::upload_2_gpu() {
  if (!m_pRenderTarget)
    return;

  if (!m_pGpuTexture) {
    m_pGpuTexture = static_cast<ui::HardwareCompositor *>(m_pCompositor)
                        ->CreateGpuLayerTexture();
    if (m_pGpuTexture) {
      m_pGpuTexture->Resize(m_size.width, m_size.height);
    }
  }

  Rect rc = {0, 0, (int)m_size.width, (int)m_size.height};
  m_pRenderTarget->Upload2Gpu(m_pGpuTexture, &rc, 1);
}

void HardwareLayer::virtualOnSize(uint nWidth, uint nHeight) {
  if (m_pGpuTexture) {
    m_pGpuTexture->Resize(nWidth, nHeight);
  }
  m_transfrom3d.set_size(nWidth, nHeight);
}

void HardwareLayer::Commit(GpuLayerCommitContext *pContext) {
  if (!m_pGpuTexture)
    return;
  if (!m_pLayerContent)
    return;

#if 0 // defined(OS_WIN)
  Rect rcWnd;
  m_pLayerContent->GetWindowRect(&rcWnd);
  if (rcWnd.IsEmpty())
    return;

  pContext->SetOffset(rcWnd.left, rcWnd.top);

  Rect rcParentWnd = {0};
  if (m_bClipLayerInParentObj && m_pCompositor->GetRootLayer() != this) {
    m_pLayerContent->GetParentWindowRect(&rcParentWnd);
  } else {
    ::GetClientRect(m_pCompositor->GetHWND(), &rcParentWnd);
  }
  pContext->SetClipRect(&rcParentWnd);

  if (m_nOpacity_Render != 255) {
    pContext->MultiAlpha(m_nOpacity_Render);
  }

  // 绕自身中心旋转时，需要知道这个对象在屏幕中的位置，然后才能计算出真正的旋转矩阵。
  // 因此每次使用前设置一次。
  m_transfrom3d.set_pos(rcWnd.left, rcWnd.top);

  if (!m_transfrom3d.is_identity()) {
    MATRIX44 mat;
    m_transfrom3d.get_matrix(&mat);
    m_pGpuTexture->Compositor(pContext, (float *)&mat);
  } else {
    m_pGpuTexture->Compositor(pContext, nullptr);
  }
#else
  UIASSERT(false);
#endif
}

void HardwareLayer::MapView2Layer(Point *pPoint) {
  if (!m_transfrom3d.is_identity()) {
    m_transfrom3d.mappoint_view_2_layer(pPoint);
  }
}

} // namespace ui