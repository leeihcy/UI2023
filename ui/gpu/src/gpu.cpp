#include "gpu/include/api.h"
#include "base/command_line.h"

#include "d3d10/d3d10_gpu.h"
#include "d3d11/d3d11_gpu.h"
#include "d3d12/d3d12_gpu.h"
#include "vulkan/vk_gpu.h"
#include "metal2/metal2_gpu.h"

namespace ui {

#if defined(ENABLE_METAL2)
std::unique_ptr<ui::IGpuCompositor>
Metal2CreateGpuComposition(ui::IGpuCompositorWindow *window);
bool Metal2GpuStartup();
void Metal2GpuShutdown();
#endif

static void ReleaseGpuCompositorWindow(IGpuCompositor *p) { delete p; }


// 判断本次要使用的API引擎。
GpuApi GetGpuApi() {
  static ui::GpuApi api = ui::GpuApi::Unknown;
  if (api != ui::GpuApi::Unknown) {
    return api;
  }

  // 1. 检测进程启动参数：
  CommandLine cmd;
  cmd.InitForCurrentProcess();

  std::string arg = cmd.GetSwitchValue(SWITCH_GPU_API);
  std::map<std::string, GpuApi> api_map = {
    {"d3d10", GpuApi::D3d10},
    {"d3d11", GpuApi::D3d11},
    {"d3d12", GpuApi::D3d12},
    {"metal2", GpuApi::Metal2},
    {"vulkan", GpuApi::Vulkan},
  };
  if (!arg.empty()) {
    auto iter = api_map.find(arg);
    if (iter != api_map.end()) {
      api = iter->second;
      return api;
    }
  }

  // 2. 默认优先级
  // 在macOS上，Vulkan实际上是通过MoltenVK中间层调用Metal。
  
#if defined(ENABLE_METAL2)
  api = ui::GpuApi::Metal2;
#elif defined(ENABLE_D3D12)
  api = ui::GpuApi::D3d12;
#elif defined(ENABLE_D3D11)
  api = ui::GpuApi::D3d11;
#elif defined(ENABLE_D3D10)
  api = ui::GpuApi::D3d10;
#else
  api = ui::GpuApi::Vulkan;
#endif
  return api;
}

UIGPUAPI std::shared_ptr<IGpuCompositor>
CreateGpuComposition(IGpuCompositorWindow *window) {
  std::shared_ptr<IGpuCompositor> p;

  GpuApi api = GetGpuApi();
  std::unique_ptr<IGpuCompositor> compositor;

  if (api == GpuApi::D3d10) {
#if defined(ENABLE_D3D10)
    compositor = d3d10::CreateCompsitor(window);
#endif
  } else if (api == GpuApi::D3d11) {
#if defined(ENABLE_D3D11)
    compositor = d3d11::CreateCompsitor(window);
#endif
  } else if (api == GpuApi::D3d12) {
#if defined(ENABLE_D3D12)
    compositor = d3d12::CreateCompsitor(window);
#endif
  } else if (api == GpuApi::Metal2) {
#if defined(ENABLE_METAL2)
    compositor = metal2::CreateCompsitor(window);
#endif
  } else if (api == GpuApi::Vulkan) {
#if defined(ENABLE_VULKAN)
    compositor = vulkan::CreateCompsitor(window);
#endif
  }
  return std::shared_ptr<IGpuCompositor>(compositor.release(),
                                         ReleaseGpuCompositorWindow);
}

static std::atomic_int g_startup = GPU_STARTUP_STATE::NOT_START;
extern "C" {
UIGPUAPI bool GpuStartup() {
  g_startup = GPU_STARTUP_STATE::STARTING;

  GpuApi api = GetGpuApi();
  if (api == GpuApi::D3d10) {
#if defined(ENABLE_D3D10)
    g_startup = d3d10::Startup();
#endif
  } else if (api == GpuApi::D3d11) {
#if defined(ENABLE_D3D11)
    g_startup = d3d11::Startup();
#endif
  } else if (api == GpuApi::D3d12) {
#if defined(ENABLE_D3D12)
    g_startup = d3d12::Startup();
#endif
  } else if (api == GpuApi::Metal2) {
#if defined(ENABLE_METAL2)
    g_startup = metal2::Startup();
#endif
  } else if (api == GpuApi::Vulkan) {
#if defined(ENABLE_VULKAN)
   g_startup = vulkan::Startup();
#endif
  }
  return g_startup;
}

UIGPUAPI void GpuShutdown() {
  GpuApi api = GetGpuApi();
  if (api == GpuApi::D3d10) {
#if defined(ENABLE_D3D10)
    d3d10::Shutdown();
#endif
  } else if (api == GpuApi::D3d11) {
#if defined(ENABLE_D3D11)
    d3d11::Shutdown();
#endif
  } else if (api == GpuApi::D3d12) {
#if defined(ENABLE_D3D12)
    d3d12::Shutdown();
#endif
  } else if (api == GpuApi::Metal2) {
#if defined(ENABLE_METAL2)
    metal2::Shutdown();
#endif
  } else if (api == GpuApi::Vulkan) {
#if defined(ENABLE_VULKAN)
    vulkan::Shutdown();
#endif
  }
  g_startup = GPU_STARTUP_STATE::NOT_START;
}

UIGPUAPI GPU_STARTUP_STATE GetGpuStartupState() {
  return (GPU_STARTUP_STATE)(int)g_startup;
}

} // extern "C"
} // namespace ui
