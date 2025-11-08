#include "src/vulkan/vkobjects.h"
#include "src/vulkan/vkbridge.h"
#include <assert.h>

namespace vulkan {

ImageView::ImageView(IVulkanBridge& bridge)
    : m_bridget(bridge) {}

ImageView::~ImageView() {
  Destroy();
}

bool ImageView::Create(VkImage image, VkFormat image_format) {
  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = image;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = image_format;
  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(m_bridget.GetVkDevice(), &createInfo, nullptr,
                        &m_image_view) != VK_SUCCESS) {
    printf("failed to create image views!");
    return false;
  }
  return true;
}

void ImageView::Destroy() {
   if (VK_NULL_HANDLE != m_image_view) {
    vkDestroyImageView(m_bridget.GetVkDevice(), m_image_view, nullptr);
    // image_type_ = IMAGE_TYPE_INVALID;
    m_image_view = VK_NULL_HANDLE;
  }

}

// ----------------------------------------------------------------------------


RenderPass::RenderPass(IVulkanBridge& bridge) : m_bridge(bridge) {}
RenderPass::~RenderPass() { Destroy(); }

bool RenderPass::Create(VkFormat format) {
  // attachment就是画布，即颜色、深度、模板数据最终要被写入的内存区域，通常是VkImage

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = format; // m_swapchain.ImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

  // 在开始渲染之前，需要如何处理这块画布
  // LOAD: 保留现有内容（需要读取内存数据）
  // CLEAR: 使用背景色填充。颜色在vkCmdBeginRenderPass中指定。
  // DONT_CARE: 不处理
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

  // 渲染完成后，要如何处理
  // STORE: 保存渲染结果（需要写入内存）
  // DONT_CARE: 丢弃渲染结果。如深度、模板彖
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(m_bridge.GetVkDevice(), &renderPassInfo, nullptr,
                         &m_renderpass) != VK_SUCCESS) {
    return false;
  }
  return true;
}
void RenderPass::Destroy() {
  if (m_renderpass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(m_bridge.GetVkDevice(), m_renderpass, nullptr);
    m_renderpass = VK_NULL_HANDLE;
  }
}

// ---------------------------------------------------------------------


CommandPool::CommandPool(IVulkanBridge& bridge)
    : m_bridge(bridge) {}

CommandPool::~CommandPool() {
  assert(VK_NULL_HANDLE==m_handle);
}


bool CommandPool::Initialize() {
  VkCommandPoolCreateInfo command_pool_create_info = {};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex =
      m_bridge.GetGraphicsQueueFamily();

  VkResult result =
      vkCreateCommandPool(m_bridge.GetVkDevice(),
                          &command_pool_create_info, nullptr, &m_handle);
  if (VK_SUCCESS != result) {
    printf("[Error] vkCreateCommandPool() failed: %d\n", result);
    return false;
  }

  return true;
}

void CommandPool::Destroy() {
  if (VK_NULL_HANDLE != m_handle) {
    vkDestroyCommandPool(m_bridge.GetVkDevice(), m_handle, nullptr);
    m_handle = VK_NULL_HANDLE;
  }
}

std::unique_ptr<CommandBuffer> CommandPool::CreateBuffer() {
  CommandBuffer* command_buffer = new CommandBuffer(m_bridge, true);
  command_buffer->alloc();
  return std::unique_ptr<CommandBuffer>(command_buffer);
}


} // namespace vulkan