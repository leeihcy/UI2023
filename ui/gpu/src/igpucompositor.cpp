#include "gpu/include/api.h"
#include <atomic>

#if defined(ENABLE_D3D10)
#include "d3d10/d3d10_app.h"
#include "d3d10/d3d10_compositor.h"

#elif defined(ENABLE_D3D11)
#include "d3d11/d3d11_app.h"
#include "d3d11/d3d11_compositor.h"

#elif defined(ENABLE_D3D12)
#include "d3d12/d3d12_app.h"
#include "d3d12/d3d12_compositor.h"

#elif defined(ENABLE_METAL2)
#include "metal2/metal2_app.h"
#include "metal2/metal2_compositor.h"

#elif defined(ENABLE_VULKAN)
#include "vulkan/vkapp.h"
#include "vulkan/vkcompositor.h"

#endif

namespace ui {

static void ReleaseGpuCompositorWindow(IGpuCompositor *p) {
  if (p) {
#if defined(ENABLE_D3D10)
    delete static_cast<D3D10Compositor *>(p);
#elif defined(ENABLE_D3D11)
    delete static_cast<D3D11Compositor *>(p);
#elif defined(ENABLE_D3D12)
    delete static_cast<D3D12Compositor *>(p);
#elif defined(ENABLE_METAL2)
    delete static_cast<Metal2Compositor *>(p);
#elif defined (ENABLE_VULKAN)
    delete static_cast<VulkanCompositor *>(p);
#endif
  }
}

UIGPUAPI std::shared_ptr<IGpuCompositor>
CreateGpuComposition(IGpuCompositorWindow *window) {
  std::shared_ptr<IGpuCompositor> p;

#if defined(ENABLE_D3D10)
  auto *c = new D3D10Compositor();
#elif defined(ENABLE_D3D11)
  auto *c = new D3D11Compositor();
#elif defined(ENABLE_D3D12)
  auto *c = new D3D12Compositor();
#elif defined(ENABLE_METAL2)
  auto *c = new Metal2Compositor();
#elif defined(ENABLE_VULKAN)
  auto *c = new VulkanCompositor();
#endif
  if (!c->Initialize(window)) {
    return nullptr;
  }
  return std::shared_ptr<IGpuCompositor>(c, ReleaseGpuCompositorWindow);
}

static std::atomic_int g_startup = GPU_STARTUP_STATE::NOT_START;
extern "C" {
UIGPUAPI bool GpuStartup() {
  g_startup = GPU_STARTUP_STATE::STARTING;

#if defined(ENABLE_D3D10)
  g_startup = D3D10Application::GetInstance().Startup();
#elif defined(ENABLE_D3D11)
  g_startup = D3D11Application::GetInstance().Startup();
#elif defined(ENABLE_D3D12)
  g_startup = D3D12Application::GetInstance().Startup();
#elif defined(ENABLE_METAL2)
  g_startup = Metal2Application::GetInstance().Startup();
#elif defined(ENABLE_VULKAN)
  g_startup = VulkanApplication::GetInstance().Startup();
#endif
  return g_startup;
}

UIGPUAPI void GpuShutdown() {
#if defined(ENABLE_D3D10)
  D3D10Application::GetInstance().Shutdown();
#elif defined(ENABLE_D3D11)
  D3D11Application::GetInstance().Shutdown();
#elif defined(ENABLE_D3D12)
  D3D12Application::GetInstance().Shutdown();
#elif defined(ENABLE_METAL2)
  Metal2Application::GetInstance().Shutdown();
#else
  VulkanApplication::GetInstance().Shutdown();
#endif
}

UIGPUAPI GPU_STARTUP_STATE GetGpuStartupState() {
  return (GPU_STARTUP_STATE)(int)g_startup;
}

}
} // namespace ui