#include "vulkan_sync.h"
#include "src/vulkan/wrap/vulkan_command_pool.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <assert.h>

namespace vulkan {

InFlightFrame::InFlightFrame(IVulkanBridge &bridge) : m_bridge(bridge) {}

InFlightFrame::~InFlightFrame() { Destroy(); }

void InFlightFrame::Initialize() {
  VkFenceCreateInfo fenceInfo = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                 .flags = VK_FENCE_CREATE_SIGNALED_BIT};
  vkCreateFence(m_bridge.GetVkDevice(), &fenceInfo, nullptr, &m_fence);
}

void InFlightFrame::Destroy() {
  VkDevice device = m_bridge.GetVkDevice();

  if (VK_NULL_HANDLE != m_fence) {
    vkDestroyFence(device, m_fence, nullptr);
    m_fence = VK_NULL_HANDLE;
  }
}

bool InFlightFrame::CreateCommandBuffer() {
  assert (m_command_buffer == VK_NULL_HANDLE);
  m_command_buffer = m_bridge.GetCommandPool().CreateBuffer();
  return true;
}


GpuSemaphores::GpuSemaphores(IVulkanBridge &bridge) : m_bridge(bridge) {

}
GpuSemaphores::~GpuSemaphores() {
  Destroy();
}

GpuSemaphores::GpuSemaphores(GpuSemaphores&& o) : m_bridge(o.m_bridge) {
  this->m_semaphore_on_image_available = o.m_semaphore_on_image_available;
  this->m_semaphore_on_queue_submit_finish = o.m_semaphore_on_queue_submit_finish;
  o.m_semaphore_on_image_available = VK_NULL_HANDLE;
  o.m_semaphore_on_queue_submit_finish = VK_NULL_HANDLE;
}

void GpuSemaphores::Initialize() {
  VkDevice device = m_bridge.GetVkDevice();

  VkSemaphoreCreateInfo semaphoreInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  vkCreateSemaphore(m_bridge.GetVkDevice(), &semaphoreInfo, nullptr,
                    &m_semaphore_on_image_available);
  vkCreateSemaphore(m_bridge.GetVkDevice(), &semaphoreInfo, nullptr,
                    &m_semaphore_on_queue_submit_finish);
}

void GpuSemaphores::Destroy() {
    VkDevice device = m_bridge.GetVkDevice();

  // Destroy Semaphores.
  if (VK_NULL_HANDLE != m_semaphore_on_queue_submit_finish) {
    vkDestroySemaphore(device, m_semaphore_on_queue_submit_finish, nullptr);
    m_semaphore_on_queue_submit_finish = VK_NULL_HANDLE;
  }
  if (VK_NULL_HANDLE != m_semaphore_on_image_available) {
    vkDestroySemaphore(device, m_semaphore_on_image_available, nullptr);
    m_semaphore_on_image_available = VK_NULL_HANDLE;
  }
}

} // namespace vulkan