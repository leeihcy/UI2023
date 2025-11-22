#ifndef _UI_GPU_SRC_VULKAN_VKLAYER_H_
#define _UI_GPU_SRC_VULKAN_VKLAYER_H_
#include "src/gpu_layer.h"
#include "src/vulkan/vk_bridge.h"
#include "src/vulkan/vk_buffer.h"
#include "src/vulkan/vk_objects.h"
#include <vulkan/vulkan.h>

namespace ui {
class VulkanCompositor;

class VulkanGpuLayer : public GpuLayer {
public:
  explicit VulkanGpuLayer(vulkan::IVulkanBridge &bridge);
  ~VulkanGpuLayer();

  void Resize(int nWidth, int nHeight) override;
  void Compositor(GpuLayerCommitContext *pContext,
                  float *pMatrixTransform) override;
  TextureTile *newTile() override;
  void UploadTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                        ui::Rect &dirty_of_layer,
                        ui::GpuUploadBitmap &source) override;

private:
  void createVertexDataBuffer();
  void createTileDataBuffer();
  bool createTextureImage();
  void destroy();

  bool findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                      uint32_t *out);
  bool transitionImageLayout(VkImage image, int base_layer, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);
  void copyBufferToImage(VkBuffer buffer, VkImage image, int base_layer,
                         int left, int top, uint32_t width, uint32_t height);
  bool createTextureDescriptorset();

private:
  vulkan::IVulkanBridge &m_bridge;

  // 还未上传的layer，不走compositor，避免vulkan报错
  bool m_uploaded = false;

  // 这两个Buffer是Shader的input参数，直接走vkCmdBindVertexBuffers，
  // 不涉及 descriptor set
  vulkan::Buffer m_vertexdata_buffer;
  vulkan::Buffer m_tiledata_buffer;

  // 纹理数组
  Vk::Image m_texture_image;
  Vk::ImageView m_texture_imageview;
  VkDeviceMemory m_texture_image_memory = VK_NULL_HANDLE;
  VkDescriptorSet m_texture_descriptorset = VK_NULL_HANDLE;
};

} // namespace ui

#endif