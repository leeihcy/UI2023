#include "src/d3d12/d3d12_layer.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/d3d12_objects.h"
#include "src/d3d12/d3dx12.h"
#include "src/d3d12/inc.h"
#include "src/d3d12/d3d12_swapchain.h"
#include "src/d3d12/shader/shader_types.h"
#include "src/util.h"

namespace ui {
static ID3D12Device *GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
}

D3D12Layer::D3D12Layer(d3d12::IBridge &bridge) : m_bridge(bridge) {
  createVertexBuffer();
}

D3D12Layer::~D3D12Layer() {
  Destroy();
}

void D3D12Layer::Destroy() {
  m_bridge.DeviceWaitIdle(); // <<

  m_vertex_buffer.Release();
  m_tile_buffer.Release();
  m_texture.Release();
}

void D3D12Layer::Resize(int nWidth, int nHeight) {
  if (m_width == (int)nWidth && m_height == (int)nHeight) {
    return;
  }
  ui::Log("Metal2GpuLayer Resize: width=%d, height=%d", nWidth, nHeight);

  doCreateTile(nWidth, nHeight);

  m_width = nWidth;
  m_height = nHeight;

  createTileDataBuffer();
  createTextures();
  createShaderResourceView();
}

void D3D12Layer::Compositor(GpuLayerCommitContext *pContext,
                            float *pMatrixTransform) {
  ID3D12GraphicsCommandList *command_list = m_bridge.GetCurrentCommandList();

  command_list->IASetVertexBuffers(0, 2, m_input_buffer_view);

  // 更新模型参数LayerData，layer在世界坐标转换矩阵。
  DirectX::XMMATRIX transfrom;
  if (pMatrixTransform) {
    transfrom = DirectX::XMMATRIX(pMatrixTransform);
  } else {
    transfrom = DirectX::XMMatrixIdentity();
  }

  DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(
      (float)pContext->m_xOffset, (float)pContext->m_yOffset, 0);

  d3d12::LayerData layer_data;
  layer_data.model = DirectX::XMMatrixTranspose(
      DirectX::XMMatrixMultiply(translation, transfrom));

  command_list->SetGraphicsRoot32BitConstants(
      ROOT_PARAMETER_LAYERDATA, // 根参数索引
      16,                       // 32位值数量 (4x4矩阵=16个float)
      &layer_data.model,        // 数据指针
      0                         // 偏移（以32位值计）
  );

  // 切换绑定的纹理资源
  d3d12::SwapChain &swapchain = m_bridge.GetSwapChain();

  CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle(
      swapchain.m_srv_heap.GetGpuHandle(m_descriptor_handle_index));
  command_list->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE,
                                               srvGpuHandle);

  command_list->DrawInstanced(4, // vertex count
                              m_arrayTile.GetCount(), // instance count
                              0, 0);
}
TextureTile *D3D12Layer::newTile() { return nullptr; }

void D3D12Layer::UploadTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                                  ui::Rect &dirty_of_layer,
                                  ui::GpuUploadBitmap &bitmap) {

  m_uploaded = true;
  if (!m_texture) {
    assert(false);
    return;
  }

  constexpr int px_size = 4;
  int w = dirty_of_tile.Width();
  int h = dirty_of_tile.Height();
  int buffer_size = w * h * px_size;

  d3d12::UploadContext upload_context(m_bridge);
  upload_context.Create();
  CComPtr<ID3D12Resource> uploadBuffer =
      upload_context.CreateUploadBuffer(buffer_size);

  char *dest_data = nullptr;
  uploadBuffer->Map(0, nullptr, (void **)&dest_data);

  unsigned char *source_data = bitmap.bits + dirty_of_layer.top * bitmap.pitch;
  source_data += dirty_of_layer.left * px_size;

  unsigned int buffer_pitch = w * px_size;
  for (int row = dirty_of_layer.top; row < dirty_of_layer.bottom; row++) {
    memcpy(dest_data, source_data, buffer_pitch);
    source_data += bitmap.pitch;
    dest_data += buffer_pitch;
  }
  uploadBuffer->Unmap(0, nullptr);

  int tile_index = row * m_arrayTile.GetCol() + col;

  D3D12_RESOURCE_BARRIER barrier_desc;
  ZeroMemory(&barrier_desc, sizeof(barrier_desc));
  barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  barrier_desc.Transition.pResource = m_texture;
  barrier_desc.Transition.Subresource = tile_index;

  if (m_texture_state != D3D12_RESOURCE_STATE_COPY_DEST) {
    barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier_desc.Transition.StateBefore = m_texture_state;
    barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    upload_context.GetCommandList()->ResourceBarrier(1, &barrier_desc);
    m_texture_state = D3D12_RESOURCE_STATE_COPY_DEST;
  }

  D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {0};
  footprint.Footprint.Width = w;
  footprint.Footprint.Height = h;
  footprint.Footprint.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  footprint.Footprint.RowPitch = 4 * w;
  footprint.Footprint.Depth = 1;
  // D3D12_RESOURCE_DESC texture_desc = m_texture->GetDesc();
  // GetDevice()->GetCopyableFootprints(&texture_desc, 0, 1, 0, &footprint,
  // nullptr, nullptr, &sizeInBytes);

  CD3DX12_TEXTURE_COPY_LOCATION dst(m_texture, tile_index); // ⭐ 目标slice索引
  CD3DX12_TEXTURE_COPY_LOCATION src(uploadBuffer, footprint);

  D3D12_BOX sourceRegion;
  sourceRegion.left = 0;
  sourceRegion.top = 0;
  sourceRegion.front = 0;
  sourceRegion.right = w;
  sourceRegion.bottom = h;
  sourceRegion.back = 1; // 对于2D纹理，深度为1

  upload_context.GetCommandList()->CopyTextureRegion(
      &dst, dirty_of_tile.left, dirty_of_tile.top, 0, &src, &sourceRegion);

  D3D12_RESOURCE_STATES old_state = m_texture_state;
  m_texture_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
  barrier_desc.Transition.StateBefore = old_state;
  barrier_desc.Transition.StateAfter = m_texture_state;
  upload_context.GetCommandList()->ResourceBarrier(1, &barrier_desc);

  upload_context.FlushAndFinish();
}

bool D3D12Layer::createVertexBuffer() {

  float s =(float)TILE_SIZE;

  // Define the geometry for a triangle.
  d3d12::VertexData triangleVertices[] = {
      // position,  color (not used) , texture coord
      // -------------------------------------------
      {{0, 0}, {0.0f, 0.0f, 0.0f}, {0, 0}},
      {{s, 0}, {1.0f, 0.0f, 0.0f}, {1, 0}},
      {{0, s}, {1.0f, 0.0f, 0.0f}, {0, 1}},
      {{s, s}, {1.0f, 1.0f, 1.0f}, {1, 1}}

      // {{0.0f, 0.25f}, {0.0f, 0.0f, 1.0f}, {0, 0}},
      // {{0.25f, -0.25f}, {0.0f, 1.0f, 0.0f}, {1, 0}},
      // {{-0.25f, -0.25f}, {1.0f, 0.0f, 0.0f}, {0, 1}},
      // {{0, -0.75f}, {1.0f, 0.0f, 0.0f}, {0, 1}},
    };

  const UINT buffer_size = sizeof(triangleVertices);

  // DEFAULT: GPU专用
  CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
  auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
  GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                       D3D12_RESOURCE_STATE_COMMON, nullptr,
                                       IID_PPV_ARGS(&m_vertex_buffer));

  d3d12::UploadContext upload_context(m_bridge);
  upload_context.UploadResource(
      m_vertex_buffer, triangleVertices, buffer_size,
      D3D12_RESOURCE_STATE_COMMON,
      D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

  // Initialize the vertex buffer view.
  m_input_buffer_view[0].BufferLocation = m_vertex_buffer->GetGPUVirtualAddress();
  m_input_buffer_view[0].StrideInBytes = sizeof(d3d12::VertexData);
  m_input_buffer_view[0].SizeInBytes = buffer_size;

  return true;
}

// 每个tile，作为一个instance，重复利用vertex buffer数据，
// 只需要让每个顶面的坐标再加上偏移量即可
void D3D12Layer::createTileDataBuffer() {
  int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();
  int count = row * col;

  std::vector<d3d12::TileData> tile_array;
  tile_array.reserve(count);

  for (int y = 0; y < m_arrayTile.GetRow(); ++y) {
    float y_offset = (float)(y * TILE_SIZE);
    for (int x = 0; x < m_arrayTile.GetCol(); ++x) {
      float x_offset = (float)(x * TILE_SIZE);

      d3d12::TileData tile = {.offset = {x_offset, y_offset}};
      tile_array.push_back(tile);
    }
  }

  const UINT buffer_size = (UINT)(tile_array.size() * sizeof(d3d12::TileData));

  // DEFAULT: GPU专用
  CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
  auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
  GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                       D3D12_RESOURCE_STATE_COMMON, nullptr,
                                       IID_PPV_ARGS(&m_tile_buffer));

  d3d12::UploadContext upload_context(m_bridge);
  upload_context.UploadResource(
      m_tile_buffer, tile_array.data(), buffer_size,
      D3D12_RESOURCE_STATE_COMMON,
      D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

  m_input_buffer_view[1].BufferLocation = m_tile_buffer->GetGPUVirtualAddress();
  m_input_buffer_view[1].StrideInBytes = sizeof(d3d12::TileData);
  m_input_buffer_view[1].SizeInBytes = buffer_size;

}
// 将所有tile texture做为数组进行创建，直接在shader中使用索引进行引用。
void D3D12Layer::createTextures() {

  // 1. 描述纹理资源
  D3D12_RESOURCE_DESC textureDesc = {};
  textureDesc.MipLevels = 1;
  textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  textureDesc.Width = TILE_SIZE;
  textureDesc.Height = TILE_SIZE;
  textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
  textureDesc.DepthOrArraySize =  m_arrayTile.GetCount();  // <<-- 纹理数组
  textureDesc.SampleDesc.Count = 1;
  textureDesc.SampleDesc.Quality = 0;
  textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
  textureDesc.Alignment = 0;

  // 2. 创建堆属性（默认堆 - GPU专用内存）
  CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

  // 3. 初始状态：通常为通用读取或渲染目标
  D3D12_RESOURCE_STATES initialState = m_texture_state;
  // initialState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

  // 4. 创建纹理资源
  HRESULT hr = GetDevice()->CreateCommittedResource(
      &heapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc, initialState,
      nullptr, // 优化清除值（可选）
      IID_PPV_ARGS(&m_texture));

  if (FAILED(hr)) {
    return;
  }

  // 5. 设置调试名称（便于调试）
  m_texture->SetName(L"D3D12Layer-Texture");
}

void D3D12Layer::createShaderResourceView() {
  d3d12::InFlightFrame& inflight_frame = m_bridge.GetCurrentInflightFrame();
  d3d12::SwapChain& swapchain = m_bridge.GetSwapChain();
  
  // 获取描述符大小
  UINT srvDescriptorSize = GetDevice()->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  // 创建 SRV 描述
  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
  srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

  srvDesc.Texture2DArray.MostDetailedMip = 0;
  srvDesc.Texture2DArray.MipLevels = 1;
  srvDesc.Texture2DArray.FirstArraySlice = 0;
  srvDesc.Texture2DArray.ArraySize = m_arrayTile.GetCount();
  srvDesc.Texture2DArray.PlaneSlice = 0;
  srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

  // 创建 SRV
  D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
  swapchain.m_srv_heap.AllocateDescriptor(cpu_handle, m_descriptor_handle_index);
  GetDevice()->CreateShaderResourceView(m_texture, &srvDesc, cpu_handle);
}

} // namespace ui