#include "vulkan_sync.h"
#include "src/vulkan/wrap/vulkan_command_pool.h"
#include <memory>

namespace vulkan {


SyncItem::SyncItem(IVulkanBridge& bridge) : m_bridge(bridge) {

}

SyncItem::~SyncItem() {
  Destroy();
}

void SyncItem::Initialize() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  vkCreateSemaphore(m_bridge.GetVkDevice(), &semaphoreInfo, nullptr,
                    &m_acquire_submit_semaphore);
  vkCreateSemaphore(m_bridge.GetVkDevice(), &semaphoreInfo, nullptr,
                    &m_submit_present_semaphore);

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  vkCreateFence(m_bridge.GetVkDevice(), &fenceInfo, nullptr, &m_fence);
}

void SyncItem::Destroy() {
  VkDevice device = m_bridge.GetVkDevice();

  // Destroy Semaphores.
  if (VK_NULL_HANDLE != m_submit_present_semaphore) {
    vkDestroySemaphore(device, m_submit_present_semaphore, nullptr);
    m_submit_present_semaphore = VK_NULL_HANDLE;
  }
  if (VK_NULL_HANDLE != m_acquire_submit_semaphore) {
    vkDestroySemaphore(device, m_acquire_submit_semaphore, nullptr);
    m_acquire_submit_semaphore = VK_NULL_HANDLE;
  }
  if (VK_NULL_HANDLE != m_fence) {
    vkDestroyFence(device, m_fence, nullptr);
    m_fence = VK_NULL_HANDLE;
  }
}

bool SyncItem::CreateCommandBuffer() {
   m_command_buffer = m_bridge.GetCommandPool().CreateBuffer();
   return true;
}

// Sync::Sync(IVulkanBridge& bridge) : m_bridge(bridge) {}

// Sync::~Sync() {
//   Destroy();
// }

// bool Sync::Initialize() {
//   for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//     std::unique_ptr<SyncItem> item = std::make_unique<SyncItem>(m_bridge);
//     m_items.push_back(item);
//   }
//   return true;
// }
 
// void Sync::Destroy() {
//   m_items.clear();
// }

} // namespace vulkan