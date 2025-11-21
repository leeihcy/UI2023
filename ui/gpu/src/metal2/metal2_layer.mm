#include "src/metal2/metal2_layer.h"
#include "src/metal2/metal2_pipeline.h"
#include "src/metal2/shaders/shader_types.h"
#include "src/util.h"
#include <simd/matrix.h>
#include <vector>

namespace ui {

Metal2GpuLayer::Metal2GpuLayer(IMetal2Bridge &bridge) : m_bridge(bridge) {}
Metal2GpuLayer::~Metal2GpuLayer() {
  if (m_vertices_buffer) {
    [m_vertices_buffer release];
    m_vertices_buffer = nullptr;
  }
  if (m_tiles_buffer) {
    [m_tiles_buffer release];
    m_tiles_buffer = nullptr;
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
  createTileBuffer();
  createTextures();
}

void Metal2GpuLayer::UpdateTileBitmap(int row, int col, ui::Rect &dirty_of_tile,
                                      ui::Rect &dirty_of_layer,
                                      ui::GpuUploadBitmap &source) {
  MTLRegion region = {0};
  region.origin.x = dirty_of_tile.left;
  region.origin.y = dirty_of_tile.top;
  region.size.width = dirty_of_tile.Width();
  region.size.height = dirty_of_tile.Height();
  region.size.depth = 1; // 这里不能写0

  const void *bits = source.bits + (dirty_of_layer.top * source.pitch +
                                    dirty_of_layer.left * 4);

  int texture_index = row * m_arrayTile.GetCol() + col;
  [m_texture_arr replaceRegion:region
                   mipmapLevel:0
                         slice:texture_index
                     withBytes:bits
                   bytesPerRow:source.pitch
                 bytesPerImage:source.pitch * source.height];
}

void Metal2GpuLayer::Compositor(GpuLayerCommitContext *pContext,
                                float *pMatrixTransform) {
  id<MTLRenderCommandEncoder> renderEncoder = m_bridge.GetRenderEncoder();

  [renderEncoder setVertexBuffer:m_vertices_buffer
                          offset:0
                         atIndex:(int)VertexShaderInput::VertexData];
  [renderEncoder setVertexBuffer:m_tiles_buffer
                          offset:0
                         atIndex:(int)VertexShaderInput::TileData];

  // 更新layer在世界坐标转换矩阵。
  LayerData layer_data;
  layer_data.model = matrix_identity_float4x4;
  if (pMatrixTransform) {
    memcpy(&layer_data.model.columns[0], pMatrixTransform,
           sizeof(layer_data.model));
  }
  simd::float4x4 matrix_translate = matrix_identity_float4x4;
  matrix_translate.columns[3][0] = pContext->m_xOffset;
  matrix_translate.columns[3][1] = pContext->m_yOffset;
  layer_data.model = matrix_multiply(layer_data.model, matrix_translate);

  [renderEncoder setVertexBytes:&layer_data.model
                         length:sizeof(simd_float4x4)
                        atIndex:(int)VertexShaderInput::LayerData];

  [renderEncoder setFragmentTexture:m_texture_arr
                            atIndex:(int)FragmentShaderInput::Texture];

  // 采用instance的方式，一次性绘制所有的tile，减少CPU/GPU指令交互
  [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip
                    vertexStart:0
                    vertexCount:4
                  instanceCount:m_arrayTile.GetCount()];
}

TextureTile *Metal2GpuLayer::newTile() {
  return nullptr;
}

// 绘制每个tile需要4个顶点（triangle strip)
void Metal2GpuLayer::createVertexBuffer() {
  float s = (float)TILE_SIZE;
  std::vector<VertexData> vertex_array = {
  // position,  color (not used) , texture coord
  // -------------------------------------------
      {{0, 0}, {0.0f, 0.0f, 1.0f}, {0, 0}},
      {{s, 0}, {0.0f, 1.0f, 0.0f}, {1, 0}},
      {{0, s}, {1.0f, 0.0f, 0.0f}, {0, 1}},
      {{s, s}, {0.0f, 1.0f, 1.0f}, {1, 1}}};

  m_vertices_buffer = [m_bridge.GetMetalDevice()
      newBufferWithBytes:vertex_array.data()
                  length:vertex_array.size() * sizeof(VertexData)
                 options:MTLResourceStorageModeShared];
}

// 每个tile，作为一个instance，重复利用vertex buffer数据，
// 只需要让每个顶面的坐标再加上偏移量即可
void Metal2GpuLayer::createTileBuffer() {
  int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();
  int count = row * col;

  std::vector<TileData> tile_array;
  tile_array.reserve(count);

  for (int y = 0; y < m_arrayTile.GetRow(); ++y) {
    float y_offset = y * TILE_SIZE;
    for (int x = 0; x < m_arrayTile.GetCol(); ++x) {
      float x_offset = x * TILE_SIZE;

      TileData tile = {.offset = {x_offset, y_offset}};
      tile_array.push_back(tile);
    }
  }

  m_tiles_buffer = [m_bridge.GetMetalDevice()
      newBufferWithBytes:tile_array.data()
                  length:tile_array.size() * sizeof(TileData)
                 options:MTLResourceStorageModeShared];
}
// 将所有tile texture做为数组进行创建，直接在shader中使用索引进行引用。
void Metal2GpuLayer::createTextures() {
  int tile_count = m_arrayTile.GetCol() * m_arrayTile.GetRow();
  if (tile_count <= 0) {
    return;
  }
  MTLTextureDescriptor *m_texture_descriptor = nullptr;
  m_texture_descriptor = [MTLTextureDescriptor new];
  m_texture_descriptor.textureType = MTLTextureType2DArray; // MTLTextureType2D;
  m_texture_descriptor.width = TILE_SIZE;
  m_texture_descriptor.height = TILE_SIZE;
  m_texture_descriptor.storageMode = MTLStorageModeManaged;
  m_texture_descriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
  m_texture_descriptor.usage = MTLTextureUsageShaderRead;
  m_texture_descriptor.arrayLength = tile_count;

  if (m_texture_arr) {
    [m_texture_arr release];
    m_texture_arr = nullptr;
  }
  m_texture_arr =
      [m_bridge.GetMetalDevice() newTextureWithDescriptor:m_texture_descriptor];

  [m_texture_descriptor release];
  m_texture_descriptor = nullptr;
}
}