#include "vklayer.h"
#include "vktexturetile.h"
#include <cmath>

namespace ui {

TextureTile *VulkanGpuLayer::newTile() { return new VkTextureTile(); }

void VulkanGpuLayer::Resize(int nWidth, int nHeight) {
  if (m_width == (int)nWidth && m_height == (int)nHeight) {
    return;
  }

  int col = (int)std::ceil((float)nWidth / TILE_SIZE);
  int row = (int)std::ceil((float)nHeight / TILE_SIZE);

  doCreateTile(row, col);

  m_width = nWidth;
  m_height = nHeight;

  // // 创建vbo
  // if (m_pVertexBuffer) {
  //   m_pVertexBuffer->Release();
  //   m_pVertexBuffer = nullptr;
  // }

  // D3D10_BUFFER_DESC BufDesc;
  // BufDesc.ByteWidth = sizeof(DXUT_SCREEN_VERTEX_10) * 4 * row * col;
  // BufDesc.Usage = D3D10_USAGE_DYNAMIC;
  // BufDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
  // BufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
  // BufDesc.MiscFlags = 0;

  // D3D10App::Get()->m_pDevice->CreateBuffer(&BufDesc, nullptr,
  // &m_pVertexBuffer);

  // // 上传一个纹理所要用的顶点数据。批处理

  // DXUT_SCREEN_VERTEX_10 *pVB = nullptr;
  // if (SUCCEEDED(
  //         m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (LPVOID *)&pVB)))
  //         {
  //   for (int y = 0; y < row; ++y) {
  //     for (int x = 0; x < col; ++x) {
  //       int x1 = x + 1;
  //       int y1 = y + 1;

  //       ui::D3DCOLORVALUE color = {1, 1, 1, 1};
  //       DXUT_SCREEN_VERTEX_10 vertices[4] = {
  //           {(float)TILE_SIZE * x, (float)TILE_SIZE * y, 0.f, color, 0, 0},
  //           {(float)TILE_SIZE * x1, (float)TILE_SIZE * y, 0.f, color, 1, 0},
  //           {(float)TILE_SIZE * x, (float)TILE_SIZE * y1, 0.f, color, 0, 1},
  //           {(float)TILE_SIZE * x1, (float)TILE_SIZE * y1, 0.f, color, 1, 1},
  //       };

  //       CopyMemory(pVB, vertices, sizeof(DXUT_SCREEN_VERTEX_10) * 4);
  //       pVB += 4;
  //     }
  //   }

  //   m_pVertexBuffer->Unmap();
  // }
}

void VulkanGpuLayer::Compositor(GpuLayerCommitContext *pContext,
                                float *pMatrixTransform) {
  
                                }

} // namespace ui