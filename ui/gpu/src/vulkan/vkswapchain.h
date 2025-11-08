#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_H_
#include "src/vulkan/vkbridge.h"
#include "src/vulkan/vkobjects.h"
#include "src/vulkan/vulkan_command_buffer.h"
#include "src/vulkan/vulkan_swap_chain_image.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vulkan {
  
// CPU最多提交可以提交多少帧给GPU
// 每套InFlight Frame都需要一套独立的资源，如CommandBuffer、UniformBuffer、DescriptorSets。
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
  VkFence m_command_buffer_fence = VK_NULL_HANDLE;
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


class SwapChain {
public:
  SwapChain(IVulkanBridge &bridge);
  ~SwapChain();

  bool Initialize(VkSurfaceKHR surface, int width, int height);
  bool CreateFrameBuffer(VkRenderPass renderpass);
  void DestroyForResize();
  void Destroy();

  void IncCurrentInflightFrame();
  void IncCurrentSemaphores();
  void SetCurrentImageIndex(uint32_t);
  uint32_t GetCurrentImageIndex();

  SwapChainImage *GetCurrentImage();
  InFlightFrame *GetCurrentInflightFrame();
  GpuSemaphores *GetCurrentSemaphores();

  VkSwapchainKHR handle() { return m_swapchain; }
  VkFormat ImageFormat() { return m_info.imageFormat; }
  VkExtent2D Extent2D() { return m_info.imageExtent; }
  uint32_t Size() { return m_info.minImageCount; }

private:
  bool createSwapchain(VkSurfaceKHR surface, int width, int height);
  bool initSwapChainImages();
  bool initInFlightFrames();

  bool queryCapabilities(VkSurfaceKHR surface, int width, int height);
  bool queryFormats(VkSurfaceKHR surface);
  bool queryPresentMode(VkSurfaceKHR surface);

private:
  IVulkanBridge &m_bridge; // raw_ptr<

  VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
  VkSwapchainCreateInfoKHR m_info;

  // swap chain中的图片列表，以及对应每个图片的其它关联数据
  std::vector<std::unique_ptr<SwapChainImage>> m_images;
  uint32_t m_current_image_index = 0;

  // 信号量数组，和images数量相同。无法使用image index，
  // 因为在image index更新前就要决定使用哪个信号量。
  std::vector<std::unique_ptr<GpuSemaphores>> m_gpu_semaphores;
  uint32_t m_current_semaphore_index = 0;

  // size == MAX_FRAMES_IN_FLIGHT
  // 限制CPU最大并发，避免向GPU提交太多Frame
  std::vector<std::unique_ptr<InFlightFrame>> m_inflight_frames;
  uint32_t m_current_inflight_frame_index = 0;
};

} // namespace vulkan

#endif