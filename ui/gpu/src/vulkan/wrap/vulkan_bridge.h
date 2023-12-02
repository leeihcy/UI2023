#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BRIDGE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BRIDGE_H_
#include "src/vulkan/wrap/vulkan_device_queue.h"
#include <vulkan/vulkan.h>

namespace vulkan {
class CommandPool;
class SwapChain;
class DeviceQueue;
class Pipeline;

struct IVulkanBridge {
  virtual VkDevice GetVkDevice() = 0;
  virtual VkPhysicalDevice GetVkPhysicalDevice() = 0;
  virtual VkRenderPass GetVkRenderPass() = 0;
  virtual VkCommandPool GetVkCommandPool() = 0;
  virtual VkPipeline GetVkPipeline() = 0;
  
  virtual vulkan::SwapChain& GetSwapChain() = 0;
  virtual vulkan::CommandPool& GetCommandPool() = 0;
  virtual vulkan::DeviceQueue& GetDeviceQueue() = 0;
  virtual vulkan::Pipeline& GetPipeline() = 0;
  virtual int GetGraphicsQueueFamily() = 0;
  virtual int GetPresentQueueFamily() = 0;
}; 

}

#endif