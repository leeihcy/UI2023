#ifndef _UI_GPU_SRC_VULKAN_VKLAYER_H_
#define _UI_GPU_SRC_VULKAN_VKLAYER_H_
#include "src/gpu_layer.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include "wrap/vulkan_buffer.h"

namespace ui
{
class VulkanCompositor;

class VulkanGpuLayer : public GpuLayer {
public:
  explicit VulkanGpuLayer(vulkan::IVulkanBridge& bridge);
  ~VulkanGpuLayer();

  void Resize(int nWidth, int nHeight) override;
  void Compositor(GpuLayerCommitContext *pContext, float *pMatrixTransform) override;
  TextureTile* newTile() override;

  void on_compositor(VkCommandBuffer buffer);

private:
  void createVertexBuffer();
  void createIndexBuffer();                  

private:
  vulkan::IVulkanBridge& m_bridge;

  vulkan::Buffer m_vertexBuffer;
  vulkan::Buffer m_indexBuffer;
};

} // namespace ui

#endif