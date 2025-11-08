#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_IMAGE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_IMAGE_H_
#include "src/vulkan/vkbridge.h"
#include "src/vulkan/vkobjects.h"
#include "src/vulkan/vulkan_buffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace vulkan {

// swapchain中的一帧。
class SwapChainFrame {
public:
  explicit SwapChainFrame(IVulkanBridge& bridge, VkImage swapchain_images);
  ~SwapChainFrame();

  bool Create(VkFormat imageFormat);
  bool CreateFrameBuffer(int width, int height, VkRenderPass render_pass);
  bool CreateDescriptorSet();
  bool CreateUniformBuffer();

  void UpdateUniformBuffer(uint32_t currentImage, VkCommandBuffer command_buffer);
  void UpdatePushData(VkCommandBuffer &command_buffer, glm::mat4 &mat4);

public:
  IVulkanBridge& m_bridge;

  // swap chain中创建的image，不需要释放
  // 画布。存储像素数据的内存块。
  VkImage m_swapchain_image_ref = VK_NULL_HANDLE;

  // 画布的一个特定视图，它定义了是用整块画布，还是只用画布的一个角落。
  Vk::ImageView m_image_view;

  // frame buffer是一个包含了多个图像的集合，这此图像可是以颜色、深度或模板缓冲区。
  // 在这里，我们只包含一个color attachment (image view).
  //
  // 需要为SwapChain中的每一个Image创建一个FrameBuffer。
  Vk::Framebuffer m_frame_buffer;

  VkDescriptorSet m_uniform_descriptor_sets;
  vulkan::Buffer m_uniform_buffer;
};

} // namespace vulkan

#endif