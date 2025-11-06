#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_RENDERPASS_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_RENDERPASS_H_
#include "src/vulkan/wrap/vulkan_bridge.h"

namespace vulkan {

class RenderPass {
public:
  RenderPass(IVulkanBridge& bridge);
  ~RenderPass();

  bool Create(VkFormat format);
  void Destroy();
  
  VkRenderPass handle() { return m_renderpass; }

private:
  IVulkanBridge& m_bridge;
  VkRenderPass m_renderpass = VK_NULL_HANDLE;
};

}

#endif