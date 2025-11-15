// ENABLE_METAL2

#include "metal2/metal2_app.h"
#include "metal2/metal2_compositor.h"

namespace ui {

static void ReleaseGpuCompositorWindow(IGpuCompositor *p) {
  delete static_cast<Metal2Compositor *>(p);
}

UIGPUAPI std::shared_ptr<IGpuCompositor>
CreateGpuComposition(IGpuCompositorWindow *window) {
  std::shared_ptr<IGpuCompositor> p;

  auto *c = new Metal2Compositor();
  if (!c->Initialize(window)) {
    return nullptr;
  }
  return std::shared_ptr<IGpuCompositor>(c, ReleaseGpuCompositorWindow);
}

static std::atomic_int g_startup = GPU_STARTUP_STATE::NOT_START;
extern "C" {
UIGPUAPI bool GpuStartup() {
  g_startup = GPU_STARTUP_STATE::STARTING;
  g_startup = Metal2Application::GetInstance().Startup();
  return g_startup;
}

UIGPUAPI void GpuShutdown() {
  Metal2Application::GetInstance().Shutdown();
}

UIGPUAPI GPU_STARTUP_STATE GetGpuStartupState() {
  return (GPU_STARTUP_STATE)(int)g_startup;
}
}
} // namespace ui