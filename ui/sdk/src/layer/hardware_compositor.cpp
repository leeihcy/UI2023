#include "hardware_compositor.h"
#include "hardware_layer.h"
#include "include/inc.h"
#include "include/util/log.h"
#include "base/stopwatch.h"
#include "gpu/include/api.h"
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
void HardwareCompositor::onBindHWND(WINDOW_HANDLE hWnd) {
  UIASSERT(!m_gpu_composition);
#if defined(OS_WIN)
  HMODULE hModule = GetModuleHandle(L"uigpu.dll");
  if (!hModule) {
    hModule = ::LoadLibraryA("uigpu.dll");
    if (!hModule) {
      return;
    }
  }

  typedef IGpuCompositor *(*pfnUICreateHardwareComposition)(HWND hWnd);
  pfnUICreateHardwareComposition fn =
      (pfnUICreateHardwareComposition)::GetProcAddress(
          hModule, "UICreateHardwareComposition");

  if (!fn) {
    UIASSERT(0);
    return;
  }

  m_gpu_composition = fn((HWND)m_hWnd);
#else
  m_gpu_composition = ui::CreateGpuComposition(m_hWnd);
#endif
}

void HardwareCompositor::Resize(uint nWidth, uint nSize) {
  if (m_gpu_composition)
    m_gpu_composition->Resize(nWidth, nSize);
}

Layer *HardwareCompositor::createLayerObject() { return new HardwareLayer(); }

// 硬件合成只能是每个层分别去调用updatedirty，而不是像软件渲染一样由parent
// object遍历child时去调用
// updatedirty。因为硬件下父layer可能没有dirty，而子layer有dirty.
void HardwareCompositor::UpdateDirty(RectRegion *outArrDirtyInWindow) {
  if (!m_pRootLayer)
    return;

  Layer *p = m_pRootLayer;
  // p->UpdateDirty();

  while (p) {
    update_dirty_recursion(p);
    p = p->GetNext();
  }
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
  // FillRect(hDC, prcInWindow, (HBRUSH)GetStockObject(GRAY_BRUSH));

  if (!m_gpu_composition)
    return;
  if (!m_pRootLayer)
    return;

  StopWatch stop_watch;

  GpuLayerCommitContext context;
  if (!m_gpu_composition->BeginCommit(&context))
    return;

  Layer *p = m_pRootLayer;
  while (p) {
    commit_recursion(p, &context);
    p = p->GetNext();
  }

  m_gpu_composition->EndCommit(&context);

  int ms = stop_watch.ElapseMsSinceLast();
  UI_LOG_INFO("hardware commit cost %d ms", ms);
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