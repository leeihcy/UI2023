#include "vulkan_swap_chain_image.h"
#include "vulkan/vulkan_core.h"

namespace vulkan {

SwapChainImage::SwapChainImage(IVulkanBridge &bridge, VkImage swapchain_image)
    : m_bridge(bridge), m_swapchain_image_ref(swapchain_image) {}

SwapChainImage::~SwapChainImage() {
  VkDevice device = m_bridge.GetVkDevice();

  if (VK_NULL_HANDLE != m_frame_buffer) {
    vkDestroyFramebuffer(device, m_frame_buffer, nullptr);
    m_frame_buffer = VK_NULL_HANDLE;
  }
}

SwapChainImage::SwapChainImage(SwapChainImage &&o)
    : m_bridge(o.m_bridge), m_swapchain_image_ref(o.m_swapchain_image_ref) {
  this->m_image_view.reset(o.m_image_view.release());
  this->m_frame_buffer = o.m_frame_buffer;
  o.m_frame_buffer = VK_NULL_HANDLE;
}

bool SwapChainImage::Create(VkFormat imageFormat) {
  m_image_view = std::make_unique<ImageView>(m_bridge);
  m_image_view->Initialize(m_swapchain_image_ref, imageFormat);

  return true;
}

bool SwapChainImage::CreateFrameBuffer(int width, int height,
                                       VkRenderPass render_pass) {
  VkImageView attachments[] = {m_image_view->handle()};

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

  // 这里的render_pass并不是要绑定这个render_pass，
  // 只是用这个render pass来验证Framebuffer中的attachment是否和render
  // pass中的attachment兼容。
  framebufferInfo.renderPass = render_pass;

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