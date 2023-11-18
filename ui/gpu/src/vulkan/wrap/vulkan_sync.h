#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SYNC_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SYNC_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vulkan {
class DeviceQueue;

#define MAX_FRAMES_IN_FLIGHT 2

class SyncItem {
public:
  SyncItem(IVulkanBridge& bridge);
  ~SyncItem();

  void Initialize();
  void Destroy();
  
  bool CreateCommandBuffer();
  
public:
  IVulkanBridge& m_bridge;

  std::unique_ptr<CommandBuffer> m_command_buffer;

  // 用于同步 acquire -> submit 这两个操作
  VkSemaphore m_acquire_submit_semaphore = VK_NULL_HANDLE;
  // 用于同步 submit -> Present 这两个操作
  VkSemaphore m_submit_present_semaphore = VK_NULL_HANDLE;

  VkFence m_fence = VK_NULL_HANDLE;
};

// class Sync {
// public:
//   Sync(IVulkanBridge& bridge);
//   ~Sync();

//   bool Initialize();
//   void Destroy();

// private:
//   IVulkanBridge& m_bridge;
// };

}

#endif