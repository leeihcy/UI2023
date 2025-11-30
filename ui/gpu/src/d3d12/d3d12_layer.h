#ifndef _UI_GPU_SRC_D3D12_D3D12LAYER_H_
#define _UI_GPU_SRC_D3D12_D3D12LAYER_H_
#include "src/gpu_layer.h"
#include "src/d3d12/inc.h"

namespace ui {

class D3D12Layer : public GpuLayer {
public:
  void Compositor(GpuLayerCommitContext *pContext,
                  float *pMatrixTransform) override;
  void Resize(int nWidth, int nHeight) override;
  TextureTile *newTile() override;
  void UploadTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                        ui::Rect &dirty_of_layer,
                        ui::GpuUploadBitmap &source) override;

private:
  bool createVertexBuffer();
  
private:
  CComPtr<ID3D12Resource> m_vertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
};

} // namespace ui

#endif