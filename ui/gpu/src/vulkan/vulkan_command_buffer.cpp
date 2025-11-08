#include "vulkan_command_buffer.h"
#include "src/vulkan/vkswapchain.h"
#include "vulkan/vulkan_core.h"

#include <assert.h>
#include <stdio.h>

namespace vulkan {

CommandBuffer::CommandBuffer(IVulkanBridge &bridge, bool primary)
    : m_is_primary(primary), m_bridge(bridge) {
}

CommandBuffer::~CommandBuffer() {
  Destroy();

  // assert(!submission_fence_.is_valid());
  assert(VK_NULL_HANDLE == m_command_buffer);
  assert(!m_is_recording);
}

bool CommandBuffer::Initialize() {
  VkResult result = VK_SUCCESS;
  VkDevice device = m_bridge.GetVkDevice();

  VkCommandBufferAllocateInfo command_buffer_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = m_bridge.GetVkCommandPool(),
      .level = m_is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                            : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
      .commandBufferCount = 1,
  };

  assert(VK_NULL_HANDLE == m_command_buffer);
  result =
      vkAllocateCommandBuffers(device, &command_buffer_info, &m_command_buffer);
  if (VK_SUCCESS != result) {
    printf("[Error] vkAllocateCommandBuffers() failed: %d\n", result);
    return false;
  }

  record_type_ = RECORD_TYPE_EMPTY;
  return true;
}

void CommandBuffer::Destroy() {
  if (m_command_buffer == VK_NULL_HANDLE) {
    return;
  }

  VkDevice device = m_bridge.GetVkDevice();
  // if (submission_fence_.is_valid()) {
  //   DCHECK(device_queue_->GetFenceHelper()->HasPassed(submission_fence_));
  //   submission_fence_ = VulkanFenceHelper::FenceHandle();
  // }

  if (VK_NULL_HANDLE != m_command_buffer) {
    vkFreeCommandBuffers(device, m_bridge.GetVkCommandPool(), 1,
                         &m_command_buffer);
    m_command_buffer = VK_NULL_HANDLE;
  }
}

bool CommandBuffer::alloc() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = m_bridge.GetVkCommandPool();
  allocInfo.commandBufferCount = 1;

  VkResult result = vkAllocateCommandBuffers(m_bridge.GetVkDevice(), &allocInfo,
                                             &m_command_buffer);
  return result == VK_SUCCESS;
}

void CommandBuffer::Reset() {
  vkResetCommandBuffer(m_command_buffer,
                       /*VkCommandBufferResetFlagBits*/ 0);
}

// 开始录制命令
void CommandBuffer::BeginRecordCommand() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(m_command_buffer, &beginInfo);
}

void CommandBuffer::BeginRenderPass(VkFramebuffer framebuffer, VkRenderPass renderpass) {
  // 背景色定义
  VkClearValue clearColor = { 
    .color = {0.0f, 0.0f, 0.0f, 1.0f}  
  };

  // 屏幕渲染范围（像是剪裁？）
  VkOffset2D offset = {0, 0};
  VkExtent2D extent = m_bridge.GetSwapChain().Extent2D();
  
  VkRenderPassBeginInfo renderPassInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = renderpass,
      .framebuffer = framebuffer,
      .renderArea = {.offset = offset,
                     .extent = extent},
      .clearValueCount = 1,
      .pClearValues = &clearColor,
  };
  vkCmdBeginRenderPass(m_command_buffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::BindPipeline(VkPipeline pipe_line) {
  vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe_line);
}

void CommandBuffer::EndRenderPass() { vkCmdEndRenderPass(m_command_buffer); }

void CommandBuffer::EndRecordCommand() { vkEndCommandBuffer(m_command_buffer); }

} // namespace vulkan