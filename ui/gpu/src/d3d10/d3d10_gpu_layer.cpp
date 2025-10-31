#include "d3d10_gpu_layer.h"
#include "d3d10_texture_tile.h"
#include "d3d10_app.h"
#include "d3d10_compositor.h"
#include "src/d3d10/d3d10_compositor.h"
#include "src/d3d10/inc.h"
#include "common/RenderStates.h"
#include "common/Effects.h"

namespace ui
{
D3D10GpuLayer::D3D10GpuLayer() {
  m_pVertexBuffer = nullptr;
}

D3D10GpuLayer::~D3D10GpuLayer()
{
  if (m_pVertexBuffer) {
    m_pVertexBuffer->Release();
    m_pVertexBuffer = nullptr;
  }
}

TextureTile* D3D10GpuLayer::newTile() {
  return new D3D10TextureTile();
}

void MultiMatrix(GpuLayerCommitContext &c, float *matrix16);

void D3D10GpuLayer::Compositor(GpuLayerCommitContext *pContext,
                                float *pMatrixTransform) {
  if (0 == m_width || 0 == m_height)
    return;
  if (!pContext)
    return;

  //     if (pTransform && pTransform->get_type() != TRANSFORM_HARD3D)
  //         return;

  ID3D10Device *pDevice = D3D10App::Get()->m_pDevice;

#if 1
  // 剪裁stencil应该是用父对象的矩阵，而不应
  // 该包含自己的旋转矩阵。这里在context里将父对象的矩阵脱离出来。
  if (pContext->m_bTransformValid) {
    // 如果是有矩阵变换，则剪裁区域可能不再是一个矩形，这时应该采用
    // 模板缓存的方式来进行剪裁

    // 1. 还原scissor区域为整个屏幕
    SIZE sizeWnd = static_cast<D3D10Compositor*>(m_pCompositor)->GetSize();
    D3D10_RECT rects;
    SetRect((LPRECT)&rects, 0, 0, sizeWnd.cx, sizeWnd.cy);
    pDevice->RSSetScissorRects(1, &rects);

    // 2. 清除当前模板缓存
    static_cast<D3D10Compositor*>(m_pCompositor)->ClearStencil();

    // 3. 禁止back buffer写入
    ID3D10BlendState *pOldBlendState = nullptr;
    pDevice->OMGetBlendState(&pOldBlendState, 0, 0);
    pDevice->OMSetBlendState(RenderStates::pBlendStateDisableWriteRenderTarget,
                             0, 0xFFFFFFFF);

    // 4. 按pContext->m_rcClip填充模板缓存为1
    pDevice->OMSetDepthStencilState(RenderStates::pStencilStateCreateClip, 1);

    Effects::m_pFxMatrix->SetMatrix((float *)pContext->m_matrixTransform);
    RECTF rcTextArg = {0, 0, 1, 1};
    RECTF rcDraw;
    rcDraw.left = (float)pContext->m_rcClip.left;
    rcDraw.top = (float)pContext->m_rcClip.top;
    rcDraw.right = (float)pContext->m_rcClip.right;
    rcDraw.bottom = (float)pContext->m_rcClip.bottom;
    D3D10App::Get()->ApplyTechnique(Effects::m_pTechFillRectMatrix, &rcDraw,
                                    &rcTextArg, 1);

    // 5. 恢复back buffer写入
    pDevice->OMSetBlendState(pOldBlendState, 0, 0xFFFFFFFF);
    if (pOldBlendState) {
      pOldBlendState->Release();
      pOldBlendState = nullptr;
    }

    // 6. 设置模板缓存函数，只允许当前模板值为1的位置通过测试（剪裁）
    pDevice->OMSetDepthStencilState(RenderStates::pStencilStateClip, 1);
  } else 
#endif
  {
    D3D10_RECT rects[1];
    memcpy(rects, &pContext->m_rcClip, sizeof(RECT));
    pDevice->RSSetScissorRects(1, rects);
  }
#if 1
  if (pMatrixTransform) {
    MultiMatrix(*pContext, pMatrixTransform);
  }
#endif

  UINT stride = sizeof(DXUT_SCREEN_VERTEX_10);
  UINT offset = 0;
  D3D10App::Get()->m_pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer,
                                                 &stride, &offset);

  ULONG row = m_arrayTile.GetRow();
  ULONG col = m_arrayTile.GetCol();

  long xOffset = 0;
  long yOffset = 0;
  for (ULONG y = 0; y < row; y++) {
    yOffset = y * TILE_SIZE;
    for (ULONG x = 0; x < col; x++) {
      xOffset = x * TILE_SIZE;

      m_arrayTile[y][x]->Compositor(xOffset, yOffset, (y * col + x) * 4,
                                    pContext);
    }
  }
#if 1
  if (pContext->m_bTransformValid) {
    // 7. 关闭模板缓存
    pDevice->OMSetDepthStencilState(RenderStates::pStencilStateDisable, 0);
  }
#endif
}

void D3D10GpuLayer::Resize(int nWidth, int nHeight) {
  if (m_width == (int)nWidth && m_height == (int)nHeight) {
    return;
  }

  doCreateTile(nWidth, nHeight);
  int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();
  
  m_width = nWidth;
  m_height = nHeight;

  // 创建vbo
  if (m_pVertexBuffer) {
    m_pVertexBuffer->Release();
    m_pVertexBuffer = nullptr;
  }

  D3D10_BUFFER_DESC BufDesc;
  BufDesc.ByteWidth = sizeof(DXUT_SCREEN_VERTEX_10) * 4 * row * col;
  BufDesc.Usage = D3D10_USAGE_DYNAMIC;
  BufDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
  BufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
  BufDesc.MiscFlags = 0;

  D3D10App::Get()->m_pDevice->CreateBuffer(&BufDesc, nullptr, &m_pVertexBuffer);

  // 上传一个纹理所要用的顶点数据。批处理

  DXUT_SCREEN_VERTEX_10 *pVB = nullptr;
  if (SUCCEEDED(
          m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (LPVOID *)&pVB))) {
    for (int y = 0; y < row; ++y) {
      for (int x = 0; x < col; ++x) {
        int x1 = x + 1;
        int y1 = y + 1;

        ui::D3DCOLORVALUE color = {1, 1, 1, 1};
        DXUT_SCREEN_VERTEX_10 vertices[4] = {
            {(float)TILE_SIZE * x, (float)TILE_SIZE * y, 0.f, color, 0, 0},
            {(float)TILE_SIZE * x1, (float)TILE_SIZE * y, 0.f, color, 1, 0},
            {(float)TILE_SIZE * x, (float)TILE_SIZE * y1, 0.f, color, 0, 1},
            {(float)TILE_SIZE * x1, (float)TILE_SIZE * y1, 0.f, color, 1, 1},
        };

        CopyMemory(pVB, vertices, sizeof(DXUT_SCREEN_VERTEX_10) * 4);
        pVB += 4;
      }
    }

    m_pVertexBuffer->Unmap();
  }
}

} // namespace ui
