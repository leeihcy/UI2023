#include "d3d12_gpu.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/d3d12_compositor.h"

namespace d3d12 {

bool Startup() { return ui::D3D12Application::GetInstance().Startup(); }

void Shutdown() { ui::D3D12Application::GetInstance().Shutdown(); }

std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window) {
  std::unique_ptr<ui::D3D12Compositor> c = std::make_unique<ui::D3D12Compositor>();
  if (c->Initialize(window)) {
    return c;
  }
  return nullptr;
}
} // namespace d3d12