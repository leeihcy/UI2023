#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_IMAGE_VIEW_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_IMAGE_VIEW_H_
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vulkan {
class DeviceQueue;
class CommandBuffer;
struct IVulkanBridge;

//
// 用于操作VkImage，例如带mipmapping的image
//
class ImageView {
public:
  ImageView(IVulkanBridge& bridge);
  ~ImageView();

  bool Create(VkImage image, VkFormat image_format);
  void Destroy();

  VkImageView handle() { return m_image_view; }
private:

private:
  IVulkanBridge& m_bridget; // raw_ptr<
  
  VkImageView m_image_view = VK_NULL_HANDLE;

};


class RenderPass {
public:
  RenderPass(IVulkanBridge& bridge);
  ~RenderPass();

  bool Create(VkFormat format);
  void Destroy();
  
  VkRenderPass handle() { return m_renderpass; }

private:
  IVulkanBridge& m_bridge;
  VkRenderPass m_renderpass = VK_NULL_HANDLE;
};


class CommandPool {
public:
  CommandPool(IVulkanBridge& bridge);
  ~CommandPool();

  bool Initialize();
  void Destroy();

  std::unique_ptr<CommandBuffer> CreateBuffer();

  VkCommandPool handle() {
    return m_handle;;
  }
  
private:
  IVulkanBridge& m_bridge; // raw_ptr<
  VkCommandPool m_handle = VK_NULL_HANDLE;
};

}

#endif