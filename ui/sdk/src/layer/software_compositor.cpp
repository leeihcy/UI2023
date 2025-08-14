// #include "stdafx.h"
#include "software_compositor.h"

#include "base/stopwatch.h"
#include "include/interface/renderlibrary.h"
#include "software_layer.h"
#include "src/window/window.h"
#include "windowrender.h"

namespace ui {
Layer *SoftwareCompositor::createLayerObject() { return new SoftwareLayer; }

// 软件渲染需要返回脏区域的窗口坐标，用于增量提交到最终窗口上面
bool SoftwareCompositor::UpdateDirty(DirtyRegion* outDirtyInWindow) {
  if (!m_root_layer)
    return false;
  
  // 先返回当前窗口脏区域
  m_root_layer->CopyDirtyRect(outDirtyInWindow);

  // 更新各个层
  return m_root_layer->UpdateDirty();
}

void SoftwareCompositor::doCommit(const RectRegion &arrDirtyInWindow) {
  if (!m_root_layer)
    return;

  uint nCount = arrDirtyInWindow.Count();
  if (!nCount) {
    return;
  }
#if 0
  // 给分层窗口提交的机会。
  if (m_window_render->m_window.virtualCommitReq())
    return;
#endif

  m_window_render->SoftwareCommit(
    m_root_layer->GetRenderTarget(), 
    arrDirtyInWindow.RectPtr2(), 
    nCount);
}

void SoftwareCompositor::Resize(uint nWidth, uint nSize) {}

} // namespace ui