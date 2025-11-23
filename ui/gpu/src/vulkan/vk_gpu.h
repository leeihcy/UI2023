#ifndef _UI_GPU_SRC_VULKAN_VKGPU_H_
#define _UI_GPU_SRC_VULKAN_VKGPU_H_
#include "include/api.h"
#include <memory>

namespace vulkan {

bool Startup();
void Shutdown();
std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window);

} // namespace d3d12
#endif