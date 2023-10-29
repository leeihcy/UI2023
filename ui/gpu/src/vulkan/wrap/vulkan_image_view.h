#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_IMAGE_VIEW_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_IMAGE_VIEW_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace vulkan {
class DeviceQueue;

//
// 用于操作VkImage，例如带mipmapping的image
//
class ImageView {
public:
  ImageView(IVulkanBridge& bridge);
  ~ImageView();

  bool Initialize(VkImage image, VkFormat image_format);
  void Destroy();

  VkImageView handle() { return m_image_view; }
private:

private:
  IVulkanBridge& m_bridget; // raw_ptr<
  
  VkImageView m_image_view = VK_NULL_HANDLE;

};

}

#endif