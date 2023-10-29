#include "vulkan_command_pool.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vulkan_device_queue.h"
#include <assert.h>
#include <stdio.h>

namespace vulkan {

CommandPool::CommandPool(IVulkanBridge& bridge)
    : m_bridge(bridge) {}

CommandPool::~CommandPool() {
  assert(0u == m_command_buffer_count);
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
  assert(0u == m_command_buffer_count);
  if (VK_NULL_HANDLE != m_handle) {
    vkDestroyCommandPool(m_bridge.GetVkDevice(), m_handle, nullptr);
    m_handle = VK_NULL_HANDLE;
  }
}

void CommandPool::IncrementCommandBufferCount() {
  m_command_buffer_count++;
}

void CommandPool::DecrementCommandBufferCount() {
  assert(m_command_buffer_count > 0);
  m_command_buffer_count--;
}

std::unique_ptr<CommandBuffer> CommandPool::CreateBuffer() {
  CommandBuffer* command_buffer = new CommandBuffer(m_bridge, true);
  command_buffer->alloc();
  return std::unique_ptr<CommandBuffer>(command_buffer);
}

}