#include "src/vulkan/wrap/vulkan_renderpass.h"

namespace vulkan {

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

} // namespace vulkan