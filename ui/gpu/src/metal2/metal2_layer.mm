#include "src/metal2/metal2_layer.h"
#include "src/metal2/metal2_pipeline.h"
#include "src/metal2/metal2_tile.h"
#include "src/metal2/shaders/shader_types.h"
#include <simd/matrix.h>
#include <vector>
#include "src/util.h"

namespace ui {

Metal2GpuLayer::Metal2GpuLayer(IMetal2Bridge &bridge) : m_bridge(bridge) {
  
}
Metal2GpuLayer::~Metal2GpuLayer() {
  if (m_vertices_buffer) {
    [m_vertices_buffer release];
    m_vertices_buffer = nullptr;
  }
  if (m_index_buffer) {
    [m_index_buffer release];
    m_index_buffer = nullptr;
  }
}

void Metal2GpuLayer::Resize(int nWidth, int nHeight) {
  if (m_width == (int)nWidth && m_height == (int)nHeight) {
    return;
  }
  ui::Log("Metal2GpuLayer Resize: width=%d, height=%d", nWidth, nHeight);

  doCreateTile(nWidth, nHeight);

  m_width = nWidth;
  m_height = nHeight;

  createVertexBuffer();
  createIndexBuffer();
}

void Metal2GpuLayer::Compositor(GpuLayerCommitContext *pContext,
                                float *pMatrixTransform) {

  id<MTLRenderCommandEncoder> renderEncoder = m_bridge.GetRenderEncoder();

  [renderEncoder setVertexBuffer:m_vertices_buffer
                            offset:0
                           atIndex: (int)VertexShaderInput::VertexData];

  // 更新layer在世界坐标转换矩阵。
  LayerData layer_data;
  layer_data.model = matrix_identity_float4x4;
  if (pMatrixTransform) {
    memcpy(&layer_data.model.columns[0], pMatrixTransform, sizeof(layer_data.model));
  }
  simd::float4x4 matrix_translate = matrix_identity_float4x4;
  matrix_translate.columns[3][0] = pContext->m_xOffset;
  matrix_translate.columns[3][1] = pContext->m_yOffset;
  layer_data.model = matrix_multiply(layer_data.model, matrix_translate);

  [renderEncoder setVertexBytes:&layer_data.model
                         length:sizeof(simd_float4x4)
                        atIndex:(int)VertexShaderInput::LayerData];

  int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();

  int index = 0;
  for (int y = 0; y < row; y++) {
    for (int x = 0; x < col; x++) {
      // 不需要偏移量，只需要顶点索引。
      static_cast<Metal2TextureTile*>(m_arrayTile[y][x])->Compositor(m_index_buffer, index, pContext);
      index += 4;
    }
  }
}

TextureTile *Metal2GpuLayer::newTile() {
  return new Metal2TextureTile(m_bridge);
}

void Metal2GpuLayer::createVertexBuffer() {
  std::vector<VertexData> vertex_array;
  for (int y = 0; y < m_arrayTile.GetRow(); ++y) {
    for (int x = 0; x < m_arrayTile.GetCol(); ++x) {
      int x1 = x + 1;
      int y1 = y + 1;

      // TBD: 还能继续优化，这里面应该有很多重复点。
      vertex_array.push_back(
          {{(float)TILE_SIZE * x, (float)TILE_SIZE * y}, {0.0f, 0.0f, 1.0f}, {0, 0}});
      vertex_array.push_back(
          {{(float)TILE_SIZE * x1, (float)TILE_SIZE * y},  {0.0f, 1.0f, 0.0f}, {1, 0}});
      vertex_array.push_back(
          {{(float)TILE_SIZE * x, (float)TILE_SIZE * y1},  {1.0f, 0.0f, 0.0f}, {0, 1}});
      vertex_array.push_back(
          {{(float)TILE_SIZE * x1, (float)TILE_SIZE * y1},  {0.0f, 1.0f, 1.0f}, {1, 1}});
    }
  }

  // Create a vertex buffer, and initialize it with the vertex data.
  m_vertices_buffer = [m_bridge.GetMetalDevice()
      newBufferWithBytes:vertex_array.data()
                  length:vertex_array.size() * sizeof(VertexData)
                 options:MTLResourceStorageModeShared];
}

void Metal2GpuLayer::createIndexBuffer() {
  std::vector<uint16_t> index_array;
  index_array.reserve(m_arrayTile.GetRow()*m_arrayTile.GetCol()*4);

  int base = 0;
  for (int y = 0; y < m_arrayTile.GetRow(); ++y) {
    for (int x = 0; x < m_arrayTile.GetCol(); ++x) {

      index_array.push_back(0 + base);
      index_array.push_back(1 + base);
      index_array.push_back(2 + base);
      index_array.push_back(3 + base);

      base += 4;
    }
  }

  m_index_buffer = [m_bridge.GetMetalDevice()
      newBufferWithBytes:index_array.data()
                  length:index_array.size() * sizeof(uint16_t)
                 options:MTLResourceStorageModeShared];
}

}