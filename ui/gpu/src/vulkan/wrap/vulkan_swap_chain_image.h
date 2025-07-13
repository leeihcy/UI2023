#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_IMAGE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_IMAGE_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "src/vulkan/wrap/vulkan_image_view.h"
#include <vulkan/vulkan.h>

namespace vulkan {

// swapchain中的一张图片。
class SwapChainImage {
public:
  explicit SwapChainImage(IVulkanBridge& bridge, VkImage image);
  ~SwapChainImage();
  SwapChainImage(SwapChainImage &&o);

  bool Create(VkFormat imageFormat);
  bool CreateFrameBuffer(int width, int height);

public:
  IVulkanBridge& m_bridge;

  // swap chain中创建的image，不需要释放
  VkImage m_image = VK_NULL_HANDLE;

  VkImageLayout image_layout = VK_IMAGE_LAYOUT_UNDEFINED;

  std::unique_ptr<ImageView> m_image_view;

  VkFramebuffer m_frame_buffer = VK_NULL_HANDLE;
};

} // namespace vulkan

#endif