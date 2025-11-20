// ENABLE_METAL2

#include "metal2/metal2_app.h"
#include "metal2/metal2_compositor.h"
#include <memory>

namespace ui {

std::unique_ptr<IGpuCompositor>
Metal2CreateGpuComposition(IGpuCompositorWindow *window) {
  std::unique_ptr<Metal2Compositor> c = std::make_unique<Metal2Compositor>();
  if (!c->Initialize(window)) {
    return nullptr;
  }
  return c;
}
bool Metal2GpuStartup() { return Metal2Application::GetInstance().Startup(); }
void Metal2GpuShutdown() { Metal2Application::GetInstance().Shutdown(); }

} // namespace ui