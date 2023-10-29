#include "vulkan_image_view.h"
#include "src/vulkan/vkapp.h"
#include "vulkan_device_queue.h"

namespace vulkan {

ImageView::ImageView(IVulkanBridge& bridge)
    : m_bridget(bridge) {}

ImageView::~ImageView() {
  Destroy();
}

bool ImageView::Initialize(VkImage image, VkFormat image_format) {
  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = image;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = image_format;
  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(m_bridget.GetVkDevice(), &createInfo, nullptr,
                        &m_image_view) != VK_SUCCESS) {
    printf("failed to create image views!");
    return false;
  }
  return true;
}

void ImageView::Destroy() {
   if (VK_NULL_HANDLE != m_image_view) {
    vkDestroyImageView(m_bridget.GetVkDevice(), m_image_view, nullptr);
    // image_type_ = IMAGE_TYPE_INVALID;
    m_image_view = VK_NULL_HANDLE;
  }

}

} // namespace vulkan