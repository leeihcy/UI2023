#ifndef _UI_GPU_SRC_VULKAN_TEXTURETILE_H_
#define _UI_GPU_SRC_VULKAN_TEXTURETILE_H_
#include "src/texture_tile.h"
#include "src/vulkan/vkbridge.h"
#include "src/vulkan/vulkan_buffer.h"
#include "src/vulkan/vkobjects.h"

#include <vulkan/vulkan.h>

namespace ui {
class VulkanCompositor;

class VkTextureTile : public TextureTile {
public:
  VkTextureTile(vulkan::IVulkanBridge& bridge);
  ~VkTextureTile();

protected:
  void Upload(ui::Rect &dirty_of_tile, ui::Rect &dirty_of_layer,
              ui::GpuUploadBitmap &source) override;
  void Compositor(long, long, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext) override;

private:
  bool create();
  bool createTextureImage();
  bool updateTextureDescriptorset();

  bool findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                      uint32_t *out);

  void copyBufferToImage(VkBuffer buffer, VkImage image, int left, int top, uint32_t width, uint32_t height);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  bool transitionImageLayout(VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout);

private:
  vulkan::IVulkanBridge& m_bridge;

  Vk::Image m_texture_image;
  Vk::ImageView m_texture_imageview;
  VkDeviceMemory m_texture_image_memory = VK_NULL_HANDLE;
  

  // 每个纹理对应一个descriptorset
  VkDescriptorSet m_texture_descriptorset = VK_NULL_HANDLE;
};

} // namespace ui

#endif