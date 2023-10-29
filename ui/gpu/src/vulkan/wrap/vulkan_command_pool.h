#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_COMMAND_POOL_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_COMMAND_POOL_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace vulkan {
class DeviceQueue;

class CommandPool {
public:
  CommandPool(IVulkanBridge& bridge);
  ~CommandPool();

  void IncrementCommandBufferCount();
  void DecrementCommandBufferCount();

  bool Initialize();
  void Destroy();

  std::unique_ptr<CommandBuffer> CreateBuffer();

  VkCommandPool handle() {
    return m_handle;;
  }
  
private:
  IVulkanBridge& m_bridge; // raw_ptr<
  
  VkCommandPool m_handle = VK_NULL_HANDLE;
  uint32_t m_command_buffer_count = 0;
};

}

#endif