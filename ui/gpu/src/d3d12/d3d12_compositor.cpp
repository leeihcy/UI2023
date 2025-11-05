#include "src/d3d12/d3d12_compositor.h"

namespace ui {

D3D12Compositor::~D3D12Compositor() {}
std::shared_ptr<IGpuLayer> D3D12Compositor::CreateLayerTexture() {
  return std::shared_ptr<IGpuLayer>();
}

bool D3D12Compositor::Initialize(IGpuCompositorWindow *window) {
  assert(window->GetType() == GpuCompositorWindowType::WindowsHWND);
  HWND hwnd = (HWND) static_cast<IGpuCompositorWindowHWND *>(window)->GetHWND();

  m_swapchain.Create(hwnd);
  return false;
}

bool D3D12Compositor::BeginCommit(GpuLayerCommitContext *) { return false; }
void D3D12Compositor::EndCommit(GpuLayerCommitContext *) {}
void D3D12Compositor::Resize(int nWidth, int nHeight) {}

} // namespace ui