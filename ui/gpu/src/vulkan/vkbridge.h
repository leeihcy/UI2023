#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BRIDGE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BRIDGE_H_

#include <vulkan/vulkan.h>
#include "src/vulkan/vulkan_device_queue.h"

namespace Vk {
class CommandPool;
}
namespace vulkan {
class CommandPool;
class SwapChain;
class DeviceQueue;
class PipeLine;

struct IVulkanBridge {
  virtual VkDevice GetVkDevice() = 0;
  virtual VkPhysicalDevice GetVkPhysicalDevice() = 0;
  virtual VkRenderPass GetVkRenderPass() = 0;
  virtual VkCommandPool GetVkCommandPool() = 0;
  virtual VkPipeline GetVkPipeline() = 0;
  virtual VkCommandBuffer GetCurrentCommandBuffer() = 0;

  virtual vulkan::SwapChain& GetSwapChain() = 0;
  virtual Vk::CommandPool& GetCommandPool() = 0;
  virtual vulkan::DeviceQueue& GetDeviceQueue() = 0;
  virtual vulkan::PipeLine& GetPipeline() = 0;
  virtual int GetGraphicsQueueFamily() = 0;
  virtual int GetPresentQueueFamily() = 0;
}; 

}

#endif