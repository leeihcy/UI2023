#ifndef _UI_GPU_SRC_D3D10_D3D10_GPU_LAYER_H_
#define _UI_GPU_SRC_D3D10_D3D10_GPU_LAYER_H_
#include "src/gpu_layer.h"

namespace ui
{

class D3D10GpuLayer : public GpuLayer {
public:
  D3D10GpuLayer();
  ~D3D10GpuLayer();

  void Resize(int nWidth, int nHeight) override;
  void Compositor(GpuLayerCommitContext *pContext, float *pMatrixTransform) override;
  TextureTile* newTile() override;

private:
  // Vertex Buffer Object
  // 将所有的tile的vbo放在一个buffer里面进行批处理，提升性能
  ID3D10Buffer *m_pVertexBuffer;
};

} // namespace ui

#endif