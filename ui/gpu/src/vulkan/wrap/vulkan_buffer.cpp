#include "vulkan_buffer.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "src/vulkan/wrap/vulkan_command_pool.h"
#include "vulkan/vulkan_core.h"
#include <memory.h>
#include <string.h>

namespace vulkan {

Buffer::Buffer(IVulkanBridge &bridge) : m_bridge(bridge) {}

Buffer::Buffer(Buffer&& o):m_bridge(o.m_bridge) {
  this->m_buffer = o.m_buffer;
  o.m_buffer = VK_NULL_HANDLE;
  this->m_buffer_memory = o.m_buffer_memory;
  o.m_buffer_memory = VK_NULL_HANDLE;
}

Buffer::~Buffer() {
  Destroy();
}

void Buffer::Destroy() {
  if (m_buffer != VK_NULL_HANDLE || m_buffer_memory != VK_NULL_HANDLE) {
    auto device = m_bridge.GetVkDevice();
    vkDeviceWaitIdle(device);

    if (m_buffer != VK_NULL_HANDLE) {
      vkDestroyBuffer(device, m_buffer, nullptr);
      m_buffer = VK_NULL_HANDLE;
    }
    if (m_buffer_memory != VK_NULL_HANDLE) {
      vkFreeMemory(device, m_buffer_memory, nullptr);
      m_buffer_memory = VK_NULL_HANDLE;
    }
  }
}

bool Buffer::findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties, uint32_t *out) {
  vulkan::DeviceQueue &device_queue = m_bridge.GetDeviceQueue();

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(device_queue.PhysicalDevice(),
                                      &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      *out = i;
      return true;
    }
  }
  return false;
}

bool Buffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties) {
  return createBuffer(size, usage, properties, m_buffer, m_buffer_memory);
}

bool Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkBuffer &buffer,
                          VkDeviceMemory &bufferMemory) {
  auto device = m_bridge.GetVkDevice();

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    return false;
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  if (!findMemoryType(memRequirements.memoryTypeBits, properties,
                      &allocInfo.memoryTypeIndex)) {
    return false;
  }

  if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    return false;
  }

  vkBindBufferMemory(device, buffer, bufferMemory, 0);
  return true;
}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                        VkDeviceSize size) {
  vulkan::CommandPool &command_pool = m_bridge.GetCommandPool();

  std::unique_ptr<vulkan::CommandBuffer> cb = command_pool.CreateBuffer();
  cb->BeginRecordCommand();
  {
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(cb->handle(), srcBuffer, dstBuffer, 1, &copyRegion);
  }
  cb->EndRecordCommand();

  vulkan::DeviceQueue &device_queue = m_bridge.GetDeviceQueue();
  device_queue.Submit(cb.get());
  device_queue.WaitIdle();
  cb->Destroy();
}

void Buffer::Create(TYPE type, void *data, int data_size) {
  
  // TBD: 优化复用？
  Destroy();

  auto device = m_bridge.GetVkDevice();

  VkDeviceSize bufferSize = data_size;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  // 该buffer可以用作一个transfer command(传输命令)的源
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  // 为了向BUFFER写入数据，需要先将BUFFER映射到CPU内存中进行读写。
  void *map_data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &map_data);
  memcpy(map_data, data, data_size);
  vkUnmapMemory(device, stagingBufferMemory);

  // 该buffer可以用作一个transfer command(传输命令)的目标

  int flags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  if (type == Buffer::VERTEX) {
    flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  } else if (type == Buffer::INDEX) {
    flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  }
  createBuffer(bufferSize, flags,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_buffer_memory);

  copyBuffer(stagingBuffer, m_buffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

// void Buffer::CreateIndexBuffer() {
//     auto device = m_bridge.GetVkDevice();

//   VkDeviceSize bufferSize = sizeof(RECT_INDICES[0]) *
//   std::size(RECT_INDICES);

//   VkBuffer stagingBuffer;
//   VkDeviceMemory stagingBufferMemory;
//   createBuffer(
//       bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                stagingBuffer, stagingBufferMemory);

//   void *data;
//   vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//   memcpy(data, RECT_INDICES, (size_t)bufferSize);
//   vkUnmapMemory(device, stagingBufferMemory);

//   createBuffer(
//       bufferSize,
//       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer,
//       m_indexBufferMemory);

//   m_compositor.VulkanCopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

//   vkDestroyBuffer(device, stagingBuffer, nullptr);
//   vkFreeMemory(device, stagingBufferMemory, nullptr);
// }

} // namespace vulkan