#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "src/vulkan/wrap/vulkan_image_view.h"
#include "src/vulkan/wrap/vulkan_swap_chain_image.h"
#include "src/vulkan/wrap/vulkan_sync.h"
#include <_types/_uint32_t.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vulkan {
 
class SwapChain {
public:
  SwapChain(IVulkanBridge &bridge);
  ~SwapChain();

  bool Initialize(VkSurfaceKHR surface, int width, int height);
  bool CreateFrameBuffer();
  bool CreateCommandBuffer();
  void Destroy();
  void IncCurrentFrame();
  void SetCurrentImageIndex(uint32_t);
  uint32_t GetCurrentImageIndex();
  
  SwapChainImage* GetCurrentImage();
  SyncItem* GetCurrentSync();

  VkSwapchainKHR handle() { return m_swapchain; }
  VkFormat ImageFormat() { return m_info.imageFormat; }
  VkExtent2D Extent2D() { return m_info.imageExtent; }
  uint32_t Size() { return m_info.minImageCount; }

private:
  bool create_swapchain(VkSurfaceKHR surface, int width, int height);
  bool init_swap_images();
  bool init_sync();

  bool query_capabilities(VkSurfaceKHR surface, int width, int height);
  bool query_formats(VkSurfaceKHR surface);
  bool query_present_mode(VkSurfaceKHR surface);

private:
  IVulkanBridge &m_bridge; // raw_ptr<

  VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
  VkSwapchainCreateInfoKHR m_info;

  // swap chain中的图w片列表，以及对应每个图片的其它关联数据
  std::vector<std::unique_ptr<SwapChainImage>> m_images;
  uint32_t m_current_image_index = 0;

  std::vector<std::unique_ptr<SyncItem>> m_sync_items;
  uint32_t m_current_frame_index = 0;

  // VkSemaphore next_present_semaphore_ = VK_NULL_HANDLE;
};

} // namespace vulkan

#endif