#ifndef _UI_GPU_SRC_D3D12_D3D12LAYER_H_
#define _UI_GPU_SRC_D3D12_D3D12LAYER_H_
#include "src/gpu_layer.h"
#include "src/d3d12/inc.h"
#include "src/d3d12/d3d12_bridge.h"

namespace ui {

class D3D12Layer : public GpuLayer {
public:
  D3D12Layer(d3d12::IBridge& bridge);
  ~D3D12Layer();
  void Destroy();
  
  void Compositor(GpuLayerCommitContext *pContext,
                  float *pMatrixTransform) override;
  void Resize(int nWidth, int nHeight) override;
  TextureTile *newTile() override;
  void UploadTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                        ui::Rect &dirty_of_layer,
                        ui::GpuUploadBitmap &source) override;

private:
  bool createVertexBuffer();
  void createTileDataBuffer();
  void createTextures();
  
private:
  d3d12::IBridge& m_bridge;
  
  CComPtr<ID3D12Resource> m_vertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

  CComPtr<ID3D12Resource> m_tile_buffer;

  CComPtr<ID3D12Resource> m_texture;
  D3D12_RESOURCE_STATES m_texture_state = D3D12_RESOURCE_STATE_COMMON;
  bool m_uploaded = false;
};

} // namespace ui

#endif