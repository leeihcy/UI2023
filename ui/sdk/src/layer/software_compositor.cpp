// #include "stdafx.h"
#include "software_compositor.h"
#include "include/interface/renderlibrary.h"
#include "software_layer.h"
#include "src/window/window.h"
#include "windowrender.h"

namespace ui {
Layer *SoftwareCompositor::virtualCreateLayer() { return new SoftwareLayer; }
// 软件渲染需要返回脏区域的窗口坐标，用于增量提交到最终窗口上面
void SoftwareCompositor::UpdateDirty(RectRegion* outArrDirtyInWindow) {
  if (!m_pRootLayer)
    return;

  // 先返回当前窗口脏区域
  m_pRootLayer->CopyDirtyRect(outArrDirtyInWindow);

  // 更新各个层
  m_pRootLayer->UpdateDirty();

  // 	Layer* p = m_pRootLayer;
  // 	while (p)
  // 	{
  // 		update_dirty_recursion(p);
  // 		p = p->GetNext();
  // 	}
}

void SoftwareCompositor::update_dirty_recursion(Layer *p) {
  UIASSERT(p);

  static_cast<SoftwareLayer *>(p)->UpdateDirty();

  p = p->GetFirstChild();
  while (p) {
    update_dirty_recursion(p);
    p = p->GetNext();
  }
}

void SoftwareCompositor::virtualCommit(const RectRegion &arrDirtyInWindow) {
  if (!m_pRootLayer)
    return;

  uint nCount = arrDirtyInWindow.GetCount();
  if (!nCount) {
    // UI_LOG_WARN(L"commit but no dirty area");
    return;
  }

  // 给分层窗口提交的机会。
  if (m_pWindowRender->m_window.virtualCommitReq())
    return;

  m_pWindowRender->m_window.Commit(
    m_pRootLayer->GetRenderTarget(), 
    arrDirtyInWindow.GetArrayPtr2(), 
    nCount);

  m_pRootLayer->GetRenderTarget()->Save("/Users/libo/Desktop/1.png");
}

void SoftwareCompositor::commit_recursion(Layer *p) {
  //     static_cast<SoftwareLayer*>(p)->Commit();
  //
  //     p = p->GetFirstChild();
  //     while (p)
  //     {
  //         commit_recursion(p);
  //         p = p->GetNext();
  //     }
}

void SoftwareCompositor::Resize(uint nWidth, uint nSize) {}

} // namespace ui