#ifndef _UI_GPU_SRC_VULKAN_VKLAYER_H_
#define _UI_GPU_SRC_VULKAN_VKLAYER_H_
#include "src/gpu_layer.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>

namespace ui
{
class VulkanCompositor;

class VulkanGpuLayer : public GpuLayer {
public:
  VulkanGpuLayer(VulkanCompositor& compositor);
  ~VulkanGpuLayer();

  void Resize(int nWidth, int nHeight) override;
  void Compositor(GpuLayerCommitContext *pContext, float *pMatrixTransform) override;
  TextureTile* newTile() override;

  void on_compositor(VkCommandBuffer buffer);

private:
  bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory);
  void createVertexBuffer();
  void createIndexBuffer();                  
  bool findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t* out);

private:
  VulkanCompositor& m_compositor;

  VkBuffer m_vertexBuffer;
  VkDeviceMemory m_vertexBufferMemory;
  VkBuffer m_indexBuffer;
  VkDeviceMemory m_indexBufferMemory;
};

} // namespace ui

#endif