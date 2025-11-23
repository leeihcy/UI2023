#include "vk_gpu.h"
#include "src/vulkan/vk_app.h"
#include "src/vulkan/vk_compositor.h"

namespace vulkan {

bool Startup() { return ui::VulkanApplication::GetInstance().Startup(); }

void Shutdown() { ui::VulkanApplication::GetInstance().Shutdown(); }

std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window) {
    std::unique_ptr<ui::VulkanCompositor> c = std::make_unique<ui::VulkanCompositor>();
    if (c->Initialize(window)) {
      return c;
    }
  return nullptr;
}
} // namespace d3d12