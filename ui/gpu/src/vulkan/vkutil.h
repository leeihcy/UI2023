#ifndef _UI_GPU_SRC_VULKAN_VKUTIL_H_
#define _UI_GPU_SRC_VULKAN_VKUTIL_H_
#include <vulkan/vulkan.h>

namespace ui {

void vulkan_copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

}
#endif