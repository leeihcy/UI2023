#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BUFFER_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BUFFER_H_
#include "src/vulkan/vkbridge.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>

namespace vulkan {

// 顶点缓存
// 索引缓存

class Buffer {
public:
  Buffer(IVulkanBridge &bridge);
  Buffer(Buffer&& o);
  ~Buffer();

  enum TYPE {
    VERTEX,
    INDEX,
  };

public:
  void Create(TYPE type, void *data, int data_size);
  void Destroy();
  
  VkBuffer handle() { return m_buffer; }
  VkDeviceMemory memory() { return m_buffer_memory; }

  bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties);
private:
  bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  bool findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                      uint32_t *out);

private:
  IVulkanBridge &m_bridge;
  VkBuffer m_buffer = VK_NULL_HANDLE;
  VkDeviceMemory m_buffer_memory = VK_NULL_HANDLE;
};

} // namespace vulkan

#endif