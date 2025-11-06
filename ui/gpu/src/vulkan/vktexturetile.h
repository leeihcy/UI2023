#ifndef _UI_GPU_SRC_VULKAN_TEXTURETILE_H_
#define _UI_GPU_SRC_VULKAN_TEXTURETILE_H_
#include "src/texture_tile.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include <vulkan/vulkan.h>
#include "vulkan/vulkan_core.h"
#include "wrap/vulkan_buffer.h"

namespace ui {
class VulkanCompositor;

class VkTextureTile : public TextureTile {
public:
  VkTextureTile(vulkan::IVulkanBridge& bridge);
  ~VkTextureTile();

protected:
  void Upload(ui::Rect &rcSrc, ui::UploadGpuBitmapInfo &source) override;
  void Compositor(long, long, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext) override;

private:
  bool create();
  bool create_vulkan_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
                            VkMemoryPropertyFlags properties, VkBuffer &buffer,
                            VkDeviceMemory &bufferMemory);
  bool create_texture_image(uint32_t width, uint32_t height, VkFormat format,
                           VkImageTiling tiling, VkImageUsageFlags usage,
                           VkMemoryPropertyFlags properties);

  bool create_texture_imageview();
  bool update_texture_descriptorset();

  bool findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                      uint32_t *out);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  bool transitionImageLayout(VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout);

private:
  vulkan::IVulkanBridge& m_bridge;

  VkImage m_texture_image = VK_NULL_HANDLE;
  VkDeviceMemory m_texture_image_memory = VK_NULL_HANDLE;

  VkImageView m_texture_imageview = VK_NULL_HANDLE;

  // 每个纹理对应一个descriptorset
  VkDescriptorSet m_texture_descriptorset = VK_NULL_HANDLE;
};

} // namespace ui

#endif