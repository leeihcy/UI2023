#include "src/d3d12/d3d12_layer.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/d3dx12.h"
#include "src/d3d12/inc.h"
#include "src/d3d12/shader/shader_types.h"


namespace ui {
static ID3D12Device *GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
}

void D3D12Layer::Compositor(GpuLayerCommitContext *pContext,
                            float *pMatrixTransform) {}
void D3D12Layer::Resize(int nWidth, int nHeight) {}
TextureTile *D3D12Layer::newTile() { return nullptr; }
void D3D12Layer::UploadTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                                  ui::Rect &dirty_of_layer,
                                  ui::GpuUploadBitmap &source) {}

bool D3D12Layer::createVertexBuffer() {
  float s = (float)TILE_SIZE;

  // Define the geometry for a triangle.
  d3d12::VertexData triangleVertices[] = {
      // position,  color (not used) , texture coord
      // -------------------------------------------
      {{0, 0}, {0.0f, 0.0f, 1.0f}, {0, 0}},
      {{s, 0}, {0.0f, 1.0f, 0.0f}, {1, 0}},
      {{0, s}, {1.0f, 0.0f, 0.0f}, {0, 1}},
      {{s, s}, {0.0f, 1.0f, 1.0f}, {1, 1}}};

  const UINT vertexBufferSize = sizeof(triangleVertices);

  // Note: using upload heaps to transfer static data like vert buffers is not
  // recommended. Every time the GPU needs it, the upload heap will be
  // marshalled over. Please read up on Default Heap usage. An upload heap is
  // used here for code simplicity and because there are very few verts to
  // actually transfer.
  CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
  auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
  GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                       D3D12_RESOURCE_STATE_GENERIC_READ,
                                       nullptr, IID_PPV_ARGS(&m_vertexBuffer));

  // Copy the triangle data to the vertex buffer.
  UINT8 *pVertexDataBegin;
  CD3DX12_RANGE readRange(
      0, 0); // We do not intend to read from this resource on the CPU.
  if (!m_vertexBuffer->Map(0, &readRange,
                           reinterpret_cast<void **>(&pVertexDataBegin))) {
    return false;
  }
  memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
  m_vertexBuffer->Unmap(0, nullptr);

  // Initialize the vertex buffer view.
  m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
  m_vertexBufferView.StrideInBytes = sizeof(d3d12::VertexData);
  m_vertexBufferView.SizeInBytes = vertexBufferSize;

  return true;
}

} // namespace ui