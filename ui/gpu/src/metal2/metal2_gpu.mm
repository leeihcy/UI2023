#include "metal2_gpu.h"
#include "src/metal2/metal2_app.h"
#include "src/metal2/metal2_compositor.h"

namespace metal2 {

bool Startup() { return ui::Metal2Application::GetInstance().Startup(); }

void Shutdown() { ui::Metal2Application::GetInstance().Shutdown(); }

std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window) {
  std::unique_ptr<ui::Metal2Compositor> c = std::make_unique<ui::Metal2Compositor>();
  if (c->Initialize(window)) {
    return c;
  }
  return nullptr;
}
} // namespace d3d12