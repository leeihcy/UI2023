#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "src/vulkan/wrap/vulkan_image_view.h"
#include "src/vulkan/wrap/vulkan_swap_chain_image.h"
#include "src/vulkan/wrap/vulkan_sync.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vulkan {

class SwapChain {
public:
  SwapChain(IVulkanBridge &bridge);
  ~SwapChain();

  bool Initialize(VkSurfaceKHR surface, int width, int height);
  bool CreateFrameBuffer(VkRenderPass renderpass);
  void DestroyForResize();
  void Destroy();

  void IncCurrentFrame();
  void IncCurrentSemaphores();
  void SetCurrentImageIndex(uint32_t);
  uint32_t GetCurrentImageIndex();

  SwapChainImage *GetCurrentImage();
  InFlightFrame *GetCurrentSync();
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

  // size == m_images.size
  // 为每个swapchain image分配一个独立的semaphore，见下面的报错log。
  // 由于调用vkAcquireNextImageKHR时还没有image index，但需要传递一个
  // semaphore，因此我们再将semaphores index独立出来。
  //
  // Here are some common methods to ensure that a semaphore passed
  // to vkQueuePresentKHR is not in use and can be safely reused:
  //         a) Use a separate semaphore per swapchain image. Index these
  //         semaphores using the index of the acquired image. 
  //         b) Consider the VK_EXT_swapchain_maintenance1 extension. It 
  //         allows using a VkFence with the presentation operation.
  // The Vulkan spec states: Each binary semaphore element of the
  // pSignalSemaphores member of any element of pSubmits must be unsignaled when
  // the semaphore signal operation it defines is executed on the device
  // (https://vulkan.lunarg.com/doc/view/1.4.313.2/windows/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pSignalSemaphores-00067)
  //
  std::vector<std::unique_ptr<GpuSemaphores>> m_gpu_semaphores;
  uint32_t m_current_semaphore_index = 0;

  // size == MAX_FRAMES_IN_FLIGHT
  // 限制CPU最大并发，避免向GPU提交太多Frame
  std::vector<std::unique_ptr<InFlightFrame>> m_inflight_frames;
  uint32_t m_current_frame_index = 0;

  // VkSemaphore next_present_semaphore_ = VK_NULL_HANDLE;
};

} // namespace vulkan

#endif