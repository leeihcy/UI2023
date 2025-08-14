#include "hardware_compositor.h"
#include "hardware_layer.h"
#include "include/inc.h"
#include "include/util/log.h"
#include "base/stopwatch.h"
#include "gpu/include/api.h"
#include "sdk/src/window/window.h"
#if defined(OS_WIN)
#include "src/util/windows.h"
#endif

namespace ui {
HardwareCompositor::HardwareCompositor() {}

HardwareCompositor::~HardwareCompositor() {
  if (m_gpu_composition) {
    m_gpu_composition->Release();
    m_gpu_composition = nullptr;
  }
}
void HardwareCompositor::onBindWindow(Window* w) {
  UIASSERT(!m_gpu_composition);

  IGpuCompositorWindow* p = w->GetWindowPlatform()->GetGpuCompositorWindow();
  m_gpu_composition = ui::CreateGpuComposition(p);

  if (!m_gpu_composition) {
    return;
  }

  // 像素大小
  ui::Rect rc;
  w->GetWindowPlatform()->GetClientRect(&rc);
  m_gpu_composition->Resize(rc.Width(), rc.Height());
}

void HardwareCompositor::Resize(uint nWidth, uint nSize) {
  if (!m_gpu_composition) {
    return;
  }
  // 像素大小
  ui::Rect rc;
  m_window_render->m_window.GetWindowPlatform()->GetClientRect(&rc);
  m_gpu_composition->Resize(rc.Width(), rc.Height());
}

Layer *HardwareCompositor::createLayerObject() { return new HardwareLayer(); }

// 硬件合成只能是每个层分别去调用updatedirty，而不是像软件渲染一样由parent
// object遍历child时去调用
// updatedirty。因为硬件下父layer可能没有dirty，而子layer有dirty.
bool HardwareCompositor::UpdateDirty(RectRegion *outArrDirtyInWindow) {
  if (!m_root_layer)
    return false;

  Layer *p = m_root_layer;
  // p->UpdateDirty();

  while (p) {
    update_dirty_recursion(p);
    p = p->GetNext();
  }
  return true;
}

void HardwareCompositor::update_dirty_recursion(Layer *p) {
  UIASSERT(p);
  static_cast<HardwareLayer *>(p)->UpdateDirty();

  p = p->GetFirstChild();
  while (p) {
    update_dirty_recursion(p);
    p = p->GetNext();
  }
}

void HardwareCompositor::doCommit(const RectRegion &arrDirtyInWindow) {
  if (!m_gpu_composition || !m_root_layer || !m_window_render)
    return;

  GpuLayerCommitContext context;
  if (!m_gpu_composition->BeginCommit(&context))
    return;

  Layer *p = m_root_layer;
  while (p) {
    commit_recursion(p, &context);
    p = p->GetNext();
  }

  m_gpu_composition->EndCommit(&context);
}

void HardwareCompositor::commit_recursion(Layer *p,
                                          GpuLayerCommitContext *pContext) {
  if (!p->GetContent()->IsVisible())
    return;

  GpuLayerCommitContext context(*pContext);
  static_cast<HardwareLayer *>(p)->Commit(&context);

  p = p->GetFirstChild();
  while (p) {
    commit_recursion(p, &context);
    p = p->GetNext();
  }
}

IGpuLayer *HardwareCompositor::CreateGpuLayerTexture() {
  UIASSERT(m_gpu_composition);
  if (!m_gpu_composition)
    return nullptr;

  IGpuLayer *pGpuTexture = nullptr;
  if (m_gpu_composition) {
    pGpuTexture = m_gpu_composition->CreateLayerTexture();
  }

  return pGpuTexture;
}

} // namespace ui