#include "hardware_compositor.h"
#include "hardware_layer.h"
#include "include/inc.h"

#if defined(OS_WIN)
#include "src/util/windows.h"
#include "gpu/include/api.h"
#endif

namespace ui {
HardwareCompositor::HardwareCompositor() { m_pHardwareComposition = nullptr; }

HardwareCompositor::~HardwareCompositor() {
  if (m_pHardwareComposition) {
    m_pHardwareComposition->Release();
    m_pHardwareComposition = nullptr;
  }
}
#if defined(OS_WIN)
void HardwareCompositor::virtualBindHWND(WINDOW_HANDLE hWnd) {
  UIASSERT(!m_pHardwareComposition);

  HMODULE hModule = GetModuleHandle(L"uigpu.dll");
  if (!hModule) {
    hModule = ::LoadLibraryA("uigpu.dll");
    if (!hModule) {
      return;
    }
  }

  typedef IHardwareComposition *(*pfnUICreateHardwareComposition)(HWND hWnd);
  pfnUICreateHardwareComposition fn =
      (pfnUICreateHardwareComposition)::GetProcAddress(
          hModule, "UICreateHardwareComposition");

  if (!fn) {
    UIASSERT(0);
    return;
  }

  m_pHardwareComposition = fn((HWND)m_hWnd);
}
#endif

void HardwareCompositor::Resize(uint nWidth, uint nSize) {
  if (m_pHardwareComposition)
    m_pHardwareComposition->Resize(nWidth, nSize);
}

Layer *HardwareCompositor::virtualCreateLayer() { return new HardwareLayer(); }

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

void HardwareCompositor::virtualCommit(const RectRegion &arrDirtyInWindow) {
  // FillRect(hDC, prcInWindow, (HBRUSH)GetStockObject(GRAY_BRUSH));

  if (!m_pHardwareComposition)
    return;
  if (!m_pRootLayer)
    return;

  if (!m_pHardwareComposition->BeginCommit())
    return;

  GpuLayerCommitContext context;
  Layer *p = m_pRootLayer;
  while (p) {
    commit_recursion(p, &context);
    p = p->GetNext();
  }

  m_pHardwareComposition->EndCommit();
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

IGpuRenderLayer *HardwareCompositor::CreateGpuLayerTexture() {
  UIASSERT(m_pHardwareComposition);
  if (!m_pHardwareComposition)
    return nullptr;

  IGpuRenderLayer *pGpuTexture = nullptr;
  if (m_pHardwareComposition) {
    pGpuTexture = m_pHardwareComposition->CreateLayerTexture();
  }

  return pGpuTexture;
}

} // namespace ui