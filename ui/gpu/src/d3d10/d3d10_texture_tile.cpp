#include "d3d10_texture_tile.h"
#include "src/d3d10/d3d10_app.h"
#include "src/d3d10/common/effects.h"


namespace ui {

D3D10TextureTile::D3D10TextureTile() {
  m_width = 0;
  m_height = 0;
  m_xIndex = 0;
  m_yIndex = 0;

  m_pTextureBuffer = nullptr;
  m_pShaderResourceView = nullptr;
}
D3D10TextureTile::~D3D10TextureTile() {
  if (m_pTextureBuffer) {
    m_pTextureBuffer->Release();
    m_pTextureBuffer = nullptr;
  }
  if (m_pShaderResourceView) {
    m_pShaderResourceView->Release();
    m_pShaderResourceView = nullptr;
  }
}

// 注：dx10开始，已经不再支持24位的格式D3DFMT_R8G8B8格式了。
// 见：https://msdn.microsoft.com/en-us/library/windows/desktop/cc308051(v=vs.85).aspx

void D3D10TextureTile::Upload(ui::Rect &rcSrc, ui::GpuUploadBitmap &source) {
  if (!m_pTextureBuffer) {
    create();
  }

  D3D10_MAPPED_TEXTURE2D mappedTexture;
  UINT nSub = D3D10CalcSubresource(0, 0, 1);

  // Create的时候没有分配显存，在Map时分配显存
  HRESULT hr =
      m_pTextureBuffer->Map(nSub, D3D10_MAP_WRITE_DISCARD, 0, &mappedTexture);
  if (FAILED(hr))
    return;

  D3D10_TEXTURE2D_DESC desc;
  m_pTextureBuffer->GetDesc(&desc);

  // 如果本次内容没有填充满Tile，则需要将空白处清0，以防上一次的脏数据干扰
  int w = rcSrc.right - rcSrc.left;
  int h = rcSrc.bottom - rcSrc.top;
  if (w != TILE_SIZE || h != TILE_SIZE) {
    byte *pTexels = (byte *)mappedTexture.pData;
    /* for (int y = 0; y < TILE_SIZE; y++)
    {
        memset(pTexels, 0, 4 * TILE_SIZE);
        pTexels += mappedTexture.RowPitch;
    } */
    memset(pTexels, 0, mappedTexture.RowPitch * TILE_SIZE);
  }

  BYTE *pSrcBits = (BYTE *)source.bits;
  byte *pTexels = (byte *)mappedTexture.pData;

  pSrcBits += rcSrc.top * source.pitch;

    for (int row = rcSrc.top; row < rcSrc.bottom; row++) {
      memcpy(pTexels, pSrcBits + (rcSrc.left * 4), w * 4);
      pSrcBits += source.pitch;
      pTexels += mappedTexture.RowPitch;
    }

  m_pTextureBuffer->Unmap(nSub);

  // #ifdef _DEBUGx
  //     static bool bDebug = false;
  //     if (bDebug)
  //     {
  //         static int i = 0;
  //         i++;
  //         TCHAR szTest[256];
  //         wprintf(szTest, TEXT("c:\\AAA\\%d.dds"),i);
  //         D3DX10SaveTextureToFile(m_pTextureBuffer, D3DX10_IFF_DDS, szTest);

  //         CImage image;
  //         image.Attach(source.hBitmap);

  //         wprintf(szTest, TEXT("c:\\AAA\\%d_(%d_%d_%d_%d).png"),i,
  //                 rcSrc.left,rcSrc.top,rcSrc.right,rcSrc.bottom);
  //         image.Save(szTest, Gdiplus::ImageFormatPNG);
  //         image.Detach();
  //     }
  // #endif
}

bool D3D10TextureTile::create() {
  if (m_pTextureBuffer || m_pShaderResourceView)
    return false;

  if (!D3D10Application::GetInstance().m_device)
    return false;

  D3D10_TEXTURE2D_DESC textureDesc;
  ZeroMemory(&textureDesc, sizeof(textureDesc));

  textureDesc.Width = TILE_SIZE;
  textureDesc.Height = TILE_SIZE;
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 1;
  textureDesc.Format =
      // DXGI_FORMAT_R8G8B8A8_UNORM
      
      // windows bitmap的RGB排列为BGRA的顺序。
      // 需要在CreateDevice时添加对应的D3D10_CREATE_DEVICE_BGRA_SUPPORT Flag
      DXGI_FORMAT_B8G8R8A8_UNORM 
  ;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.SampleDesc.Quality = 0;
  textureDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
  textureDesc.Usage = D3D10_USAGE_DYNAMIC;
  textureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
  textureDesc.MiscFlags = 0;

  HRESULT hr = D3D10Application::GetInstance().m_device->CreateTexture2D(
      &textureDesc, nullptr, &m_pTextureBuffer);
  if (FAILED(hr)) {
    assert(false);
    return false;
  }

  D3D10_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
  shaderResourceViewDesc.Format = textureDesc.Format;
  shaderResourceViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
  shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
  shaderResourceViewDesc.Texture2D.MipLevels = 1;

  hr = D3D10Application::GetInstance().m_device->CreateShaderResourceView(
      m_pTextureBuffer, &shaderResourceViewDesc, &m_pShaderResourceView);
  if (FAILED(hr)) {
    if (m_pTextureBuffer) {
      m_pTextureBuffer->Release();
      m_pTextureBuffer = nullptr;
    }
    return false;
  }

  return true;
}

// vertexStartIndex:
// 如果给每个tile单独配置一个vertex
// buffer，则会导致每次绘制之前都要切换当前vertex buffer，
// 使得渲染性能很差。因此这里采用了批处理，将所有块的vb都放在了layer对象中，然后通过索引来绘制
void D3D10TextureTile::Compositor(long xOffset, long yOffset, long vertexStartIndex,
                             ui::GpuLayerCommitContext *pContext) {
  if (!m_pShaderResourceView)
    return;

  float pos[2] = {(float)(pContext->m_xOffset), (float)(pContext->m_yOffset)};
  Effects::GetInstance().m_pFxVsDestPos->SetFloatVector(pos);

  ID3D10EffectTechnique *pTech = nullptr;
#if 1
  if (pContext->m_bTransformValid) {
    Effects::GetInstance().m_pFxMatrix->SetMatrix(
        (float *)pContext->m_matrixTransform);

    pTech = Effects::GetInstance().m_pTechDrawTextureMatrix;
  } else
#endif
   {
    pTech = Effects::GetInstance().m_pTechDrawTexture;
  }

  if (pContext->m_fAlpha != 1.0f)
    Effects::GetInstance().m_pFxAlpha->SetFloat(pContext->m_fAlpha);
  Effects::GetInstance().m_pFxTexture10->SetResource(m_pShaderResourceView);

  D3D10_TECHNIQUE_DESC techDesc;
  pTech->GetDesc(&techDesc);
  for (UINT p = 0; p < techDesc.Passes; ++p) {
    pTech->GetPassByIndex(p)->Apply(0);
    D3D10Application::GetInstance().m_device->Draw(4, vertexStartIndex);
  }

  if (pContext->m_fAlpha != 1.0f) {
    Effects::GetInstance().m_pFxAlpha->SetFloat(1.0f);
  }
}

}