#include "vulkan_swap_chain_image.h"
#include "vulkan/vulkan_core.h"



namespace vulkan {

SwapChainImage::SwapChainImage(IVulkanBridge &bridge, VkImage image)
    : m_bridge(bridge), m_image(image) {}

SwapChainImage::~SwapChainImage() {
  VkDevice device = m_bridge.GetVkDevice();

  if (VK_NULL_HANDLE != m_frame_buffer) {
    vkDestroyFramebuffer(device, m_frame_buffer, nullptr);
    m_frame_buffer = VK_NULL_HANDLE;
  }
}

SwapChainImage::SwapChainImage(SwapChainImage &&o)
    : m_bridge(o.m_bridge), m_image(o.m_image) {
  this->m_image_view.reset(o.m_image_view.release());

  this->image_layout = o.image_layout;
  this->m_frame_buffer = o.m_frame_buffer;
  o.m_frame_buffer = VK_NULL_HANDLE;
}

bool SwapChainImage::Create(VkFormat imageFormat) {
  m_image_view = std::make_unique<ImageView>(m_bridge);
  m_image_view->Initialize(m_image, imageFormat);

  return true;
}

bool SwapChainImage::CreateFrameBuffer(int width, int height) {
  VkImageView attachments[] = {m_image_view->handle()};

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = m_bridge.GetVkRenderPass();
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = attachments;
  framebufferInfo.width = width;
  framebufferInfo.height = height;
  framebufferInfo.layers = 1;

  if (vkCreateFramebuffer(m_bridge.GetVkDevice(), &framebufferInfo, nullptr,
                          &m_frame_buffer) != VK_SUCCESS) {
    return false;
  }
  return true;
}

} // namespace vulkan