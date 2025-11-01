#ifndef _UI_GPU_SRC_D3D11_D3D11_GPU_LAYER_H_
#define _UI_GPU_SRC_D3D11_D3D11_GPU_LAYER_H_
#include "src/gpu_layer.h"
#include "src/d3d11/inc.h"

namespace ui
{

class D3D11GpuLayer : public GpuLayer {
public:
  void Resize(int nWidth, int nHeight) override;
  void Compositor(GpuLayerCommitContext *pContext, float *pMatrixTransform) override;
  TextureTile* newTile() override;

private:
  // Vertex Buffer Object
  // 将所有的tile的vbo放在一个buffer里面进行批处理，提升性能
  CComPtr<ID3D11Buffer> m_pVertexBuffer;
};

} // namespace ui

#endif