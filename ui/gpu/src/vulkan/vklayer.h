#ifndef _UI_GPU_SRC_VULKAN_VKLAYER_H_
#define _UI_GPU_SRC_VULKAN_VKLAYER_H_
#include "src/gpu_layer.h"

namespace ui
{

class VulkanGpuLayer : public GpuLayer {
public:
  void Resize(int nWidth, int nHeight) override;
  void Compositor(GpuLayerCommitContext *pContext, float *pMatrixTransform) override;
  TextureTile* newTile() override;
};

} // namespace ui

#endif