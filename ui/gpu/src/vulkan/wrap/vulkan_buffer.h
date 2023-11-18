#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BUFFER_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_BUFFER_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include <vulkan/vulkan.h>

namespace vulkan {

// 顶点缓存
// 索引缓存

class Buffer {
public:
  Buffer();
  ~Buffer();
};

} // namespace vulkan

#endif