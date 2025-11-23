#include "d3d11_gpu_layer.h"
#include "common/effects.h"
#include "common/render_states.h"
#include "d3d11_app.h"
#include "d3d11_compositor.h"
#include "d3d11_texture_tile.h"
#include "src/d3d11/d3d11_compositor.h"
#include "src/d3d11/inc.h"
using namespace DirectX;

namespace ui {

TextureTile *D3D11GpuLayer::newTile() { return new D3D11TextureTile(); }

static void MultiMatrix(GpuLayerCommitContext &c, float *matrix16) {
  XMMATRIX mat1(matrix16);
  if (XMMatrixIsIdentity(mat1))
    return;

  XMMATRIX mat2((float *)c.m_matrixTransform);

  mat1 *= mat2;
  memcpy(&c.m_matrixTransform, &mat1, sizeof(mat1));

  c.m_bTransformValid = true;
}

void D3D11GpuLayer::Compositor(GpuLayerCommitContext *pContext,
                               float *pMatrixTransform) {
  if (0 == m_width || 0 == m_height)
    return;
  if (!pContext)
    return;

  //     if (pTransform && pTransform->get_type() != TRANSFORM_HARD3D)
  //         return;

  ID3D11DeviceContext *pDevice = D3D11Application::GetInstance().m_device_context;

  // 剪裁stencil应该是用父对象的矩阵，而不应
  // 该包含自己的旋转矩阵。这里在context里将父对象的矩阵脱离出来。
  if (pContext->m_bTransformValid) {
    // 如果是有矩阵变换，则剪裁区域可能不再是一个矩形，这时应该采用
    // 模板缓存的方式来进行剪裁

    // 1. 还原scissor区域为整个屏幕
    SIZE sizeWnd = static_cast<D3D11Compositor *>(m_pCompositor)->GetSize();
    D3D11_RECT rects;
    SetRect((LPRECT)&rects, 0, 0, sizeWnd.cx, sizeWnd.cy);
    pDevice->RSSetScissorRects(1, &rects);

    // 2. 清除当前模板缓存
    static_cast<D3D11Compositor *>(m_pCompositor)->ClearStencil();

    // 3. 禁止back buffer写入
    ID3D11BlendState *pOldBlendState = nullptr;
    pDevice->OMGetBlendState(&pOldBlendState, 0, 0);
    pDevice->OMSetBlendState(
        d3d11::RenderStates::GetInstance().pBlendStateDisableWriteRenderTarget, 0,
        0xFFFFFFFF);

    // 4. 按pContext->m_rcClip填充模板缓存为1
    pDevice->OMSetDepthStencilState(
        d3d11::RenderStates::GetInstance().pStencilStateCreateClip, 1);

    d3d11::Effects::GetInstance().m_pFxMatrix->SetMatrix(
        (float *)pContext->m_matrixTransform);
    RECTF rcTextArg = {0, 0, 1, 1};
    RECTF rcDraw;
    rcDraw.left = (float)pContext->m_rcClip.left;
    rcDraw.top = (float)pContext->m_rcClip.top;
    rcDraw.right = (float)pContext->m_rcClip.right;
    rcDraw.bottom = (float)pContext->m_rcClip.bottom;
    D3D11Application::GetInstance().ApplyTechnique(
        d3d11::Effects::GetInstance().m_pTechFillRectMatrix, &rcDraw, &rcTextArg, 1);

    // 5. 恢复back buffer写入
    pDevice->OMSetBlendState(pOldBlendState, 0, 0xFFFFFFFF);
    if (pOldBlendState) {
      pOldBlendState->Release();
      pOldBlendState = nullptr;
    }

    // 6. 设置模板缓存函数，只允许当前模板值为1的位置通过测试（剪裁）
    pDevice->OMSetDepthStencilState(
        d3d11::RenderStates::GetInstance().pStencilStateClip, 1);
  } else {
    D3D11_RECT rects[1];
    memcpy(rects, &pContext->m_rcClip, sizeof(RECT));
    pDevice->RSSetScissorRects(1, rects);
  }
  if (pMatrixTransform) {
    MultiMatrix(*pContext, pMatrixTransform);
  }

  const int size = 1;
  ID3D11Buffer* buffers[size] = { m_pVertexBuffer };
  UINT stride[size] = { sizeof(d3d11::DXUT_SCREEN_VERTEX_11) };
  UINT offset[size] = { 0 };
  D3D11Application::GetInstance().m_device_context->IASetVertexBuffers(
      0, size, buffers, stride, offset);

  ULONG row = m_arrayTile.GetRow();
  ULONG col = m_arrayTile.GetCol();

  long xOffset = 0;
  long yOffset = 0;
  for (ULONG y = 0; y < row; y++) {
    yOffset = y * TILE_SIZE;
    for (ULONG x = 0; x < col; x++) {
      xOffset = x * TILE_SIZE;

      static_cast<D3D11TextureTile*>(m_arrayTile[y][x])->Compositor(xOffset, yOffset, (y * col + x) * 4,
                                    pContext);
    }
  }
  if (pContext->m_bTransformValid) {
    // 7. 关闭模板缓存
    pDevice->OMSetDepthStencilState(
        d3d11::RenderStates::GetInstance().pStencilStateDisable, 0);
  }
}

void D3D11GpuLayer::Resize(int nWidth, int nHeight) {
  if (m_width == (int)nWidth && m_height == (int)nHeight) {
    return;
  }

  doCreateTile(nWidth, nHeight);
  int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();

  m_width = nWidth;
  m_height = nHeight;

  // 创建vbo
  m_pVertexBuffer.Release();

  D3D11_BUFFER_DESC BufDesc;
  BufDesc.ByteWidth = sizeof(d3d11::DXUT_SCREEN_VERTEX_11) * 4 * row * col;
  BufDesc.Usage = D3D11_USAGE_DYNAMIC;
  BufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  BufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  BufDesc.MiscFlags = 0;

  D3D11Application::GetInstance().m_device->CreateBuffer(&BufDesc, nullptr,
                                                         &m_pVertexBuffer);

  // 上传一个纹理所要用的顶点数据。批处理

  // DXUT_SCREEN_VERTEX_10 *pVB = nullptr;
  D3D11_MAPPED_SUBRESOURCE mapped_subresource;
  if (SUCCEEDED(
           D3D11Application::GetInstance().m_device_context->Map(m_pVertexBuffer, 0, 
            D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource))) {
    for (int y = 0; y < row; ++y) {
      for (int x = 0; x < col; ++x) {
        int x1 = x + 1;
        int y1 = y + 1;

        ui::D3D11COLORVALUE color = {1, 1, 1, 1};
        d3d11::DXUT_SCREEN_VERTEX_11 vertices[4] = {
            {(float)TILE_SIZE * x, (float)TILE_SIZE * y, 0.f, color, 0, 0},
            {(float)TILE_SIZE * x1, (float)TILE_SIZE * y, 0.f, color, 1, 0},
            {(float)TILE_SIZE * x, (float)TILE_SIZE * y1, 0.f, color, 0, 1},
            {(float)TILE_SIZE * x1, (float)TILE_SIZE * y1, 0.f, color, 1, 1},
        };

        CopyMemory(mapped_subresource.pData, vertices, sizeof(d3d11::DXUT_SCREEN_VERTEX_11) * 4);
        mapped_subresource.pData = (byte*)mapped_subresource.pData + sizeof(d3d11::DXUT_SCREEN_VERTEX_11) * 4;
      }
    }

    D3D11Application::GetInstance().m_device_context->Unmap(m_pVertexBuffer, 0);
  }
}

} // namespace ui
