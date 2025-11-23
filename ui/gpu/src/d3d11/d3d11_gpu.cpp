#include "d3d11_gpu.h"
#include "src/d3d11/d3d11_app.h"
#include "src/d3d11/d3d11_compositor.h"

namespace d3d11 {

bool Startup() { 
  return ui::D3D11Application::GetInstance().Startup(); 
}

void Shutdown() { 
  ui::D3D11Application::GetInstance().Shutdown();
}

std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window) {
  std::unique_ptr<ui::D3D11Compositor> c = std::make_unique<ui::D3D11Compositor>();
  if (c->Initialize(window)) {
    return c;
  }
  return nullptr;
}
} // namespace d3d11