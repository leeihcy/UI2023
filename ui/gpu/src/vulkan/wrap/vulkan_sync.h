#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SYNC_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SYNC_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vulkan {
class DeviceQueue;

// CPU最多提交可以提交多少帧给GPU
#define MAX_FRAMES_IN_FLIGHT 2

// 绘制一帧，相关的数据。
class InFlightFrame {
public:
  InFlightFrame(IVulkanBridge& bridge);
  ~InFlightFrame();

  void Initialize();
  void Destroy();
  
  bool CreateCommandBuffer();
  
public:
  IVulkanBridge& m_bridge;

  std::unique_ptr<CommandBuffer> m_command_buffer;

  // Fence，用于 GPU-CPU 同步，实现 MAX_FRAMES_IN_FLIGHT 逻辑。
  //
  // GPU 把 command buffer 通过 vkQueueSubmit 使用完成后，会将 fence 设置为 
  // Signaled，这时 CPU 可以继续提交下一帧的 command buffer。
  //
  VkFence m_fence = VK_NULL_HANDLE;
};

class GpuSemaphores {
public:
  GpuSemaphores(IVulkanBridge& bridge);
  GpuSemaphores(GpuSemaphores&& o);
  ~GpuSemaphores();

  void Initialize();
  void Destroy();
public:
  IVulkanBridge& m_bridge;

  // VkSemaphore，用于 GPU-GPU 不同操作之间的同步
  //
  // VkSemaphore 不需要手动调用 vkResetSemaphore，完全由 Vulkan 内部自动处理。
  // 用于同步 acquire -> submit 这两个操作
  VkSemaphore m_semaphore_on_image_available = VK_NULL_HANDLE;
  // 用于同步 submit -> Present 这两个操作
  VkSemaphore m_semaphore_on_queue_submit_finish = VK_NULL_HANDLE;
};

}

#endif