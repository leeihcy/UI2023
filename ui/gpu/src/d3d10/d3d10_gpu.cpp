#include "d3d10_gpu.h"
#include "src/d3d10/d3d10_app.h"
#include "src/d3d10/d3d10_compositor.h"

namespace d3d10 {

bool Startup() { 
  return ui::D3D10Application::GetInstance().Startup();
}

void Shutdown() { 
  ui::D3D10Application::GetInstance().Shutdown();
}

std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window) {
  std::unique_ptr<ui::D3D10Compositor> c = std::make_unique<ui::D3D10Compositor>();
  if (c->Initialize(window)) {
    return c;
  }
  return nullptr;
}
} // namespace d3d11