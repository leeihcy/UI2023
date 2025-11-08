#include "src/vulkan/vkobjects.h"
#include "src/vulkan/vkbridge.h"
#include <assert.h>
#include "src/util.h"

namespace Vk {

bool ImageView::Create(VkDevice device, VkImage image,
                         VkFormat image_format) {
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

  if (vkCreateImageView(device, &createInfo, nullptr, &handle) != VK_SUCCESS) {
    printf("failed to create image views!");
    return false;
  }
  return true;
}

bool RenderPass::Create(VkDevice device, VkFormat format) {
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

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr,
                         &handle) != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool CommandPool::Create(vulkan::IVulkanBridge& bridge) {
  VkCommandPoolCreateInfo command_pool_create_info = {};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex =
      bridge.GetGraphicsQueueFamily();

  VkResult result =
      vkCreateCommandPool(bridge.GetVkDevice(),
                          &command_pool_create_info, nullptr, &handle);
  if (VK_SUCCESS != result) {
    ui::Log("[Error] vkCreateCommandPool() failed: %d", result);
    return false;
  }

  return true;
}

VkCommandBuffer CommandPool::AllocateCommandBuffer(VkDevice device) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = handle;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer buffer = VK_NULL_HANDLE;
  VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &buffer);
  return buffer;
}

void CommandPool::ReleaseCommandBuffer(VkDevice device,
                                       VkCommandBuffer command_buffer) {
  if (VK_NULL_HANDLE != command_buffer) {
    vkFreeCommandBuffers(device, handle, 1, &command_buffer);
  }
}

CommandBuffer::~CommandBuffer() {
  assert(VK_NULL_HANDLE == handle);
  assert(!m_is_recording);
}

void CommandBuffer::Attach(VkCommandBuffer buffer) {
  assert(handle == VK_NULL_HANDLE);
  handle = buffer;
}

VkCommandBuffer CommandBuffer::Detach() {
  VkCommandBuffer command_buffer = handle;
  handle = VK_NULL_HANDLE;
  return command_buffer;
}

// bool CommandBuffer::Initialize() {
//   VkResult result = VK_SUCCESS;
//   VkDevice device = m_bridge.GetVkDevice();

//   VkCommandBufferAllocateInfo command_buffer_info = {
//       .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
//       .pNext = nullptr,
//       .commandPool = m_bridge.GetVkCommandPool(),
//       .level = m_is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
//                             : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
//       .commandBufferCount = 1,
//   };

//   assert(VK_NULL_HANDLE == handle);
//   result =
//       vkAllocateCommandBuffers(device, &command_buffer_info, &handle);
//   if (VK_SUCCESS != result) {
//     printf("[Error] vkAllocateCommandBuffers() failed: %d\n", result);
//     return false;
//   }
//
//   record_type_ = RECORD_TYPE_EMPTY;
//   return true;
// }

// void CommandBuffer::Destroy(VkDevice device, VkCommandPool pool) {
//   if (handle == VK_NULL_HANDLE) {
//     return;
//   }
//
//   if (VK_NULL_HANDLE != handle) {
//     vkFreeCommandBuffers(device, pool, 1, &handle);
//     handle = VK_NULL_HANDLE;
//   }
// }

void CommandBuffer::Reset() {
  vkResetCommandBuffer(handle,
                       /*VkCommandBufferResetFlagBits*/ 0);
}

// 开始录制命令
void CommandBuffer::BeginRecordCommand() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(handle, &beginInfo);
}

void CommandBuffer::BeginRenderPass(VkFramebuffer framebuffer,
                                    VkRenderPass renderpass,
                                    VkOffset2D offset,
                                    VkExtent2D extent) {
  // 背景色定义
  VkClearValue clearColor = {.color = {0.0f, 0.0f, 0.0f, 1.0f}};

  VkRenderPassBeginInfo renderPassInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = renderpass,
      .framebuffer = framebuffer,
      .renderArea = {.offset = offset,
                     .extent = extent},
      .clearValueCount = 1,
      .pClearValues = &clearColor,
  };
  vkCmdBeginRenderPass(handle, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::BindPipeline(VkPipeline pipe_line) {
  vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe_line);
}

void CommandBuffer::EndRenderPass() { vkCmdEndRenderPass(handle); }

void CommandBuffer::EndRecordCommand() { vkEndCommandBuffer(handle); }


}