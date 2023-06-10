#include "stdafx.h"
#include "texture_tile.h"
#include "common_def.h"
#include "d3d10\d3dapp.h"
#include "hard3dtransform.h"
#include <atlimage.h>
#include "d3d10\common\Effects.h"

TextureTile::TextureTile()
{
	m_width = 0;
	m_height = 0;
	m_xIndex = 0;
	m_yIndex = 0;

	m_pTextureBuffer = nullptr;
	m_pShaderResourceView = nullptr;
}
TextureTile::~TextureTile()
{
    SAFE_RELEASE(m_pTextureBuffer);
    SAFE_RELEASE(m_pShaderResourceView);
}

// ע��dx10��ʼ���Ѿ�����֧��24λ�ĸ�ʽD3DFMT_R8G8B8��ʽ�ˡ�
// ����https://msdn.microsoft.com/en-us/library/windows/desktop/cc308051(v=vs.85).aspx

void TextureTile::Upload(RECT& rcSrc, UploadGpuBitmapInfo& source)
{
    if (!m_pTextureBuffer)
    {
        create();
    }

    D3D10_MAPPED_TEXTURE2D mappedTexture;
    UINT  nSub = D3D10CalcSubresource(0, 0, 1);

    // Create��ʱ��û�з����Դ棬��Mapʱ�����Դ�
    HRESULT hr = m_pTextureBuffer->Map(nSub, D3D10_MAP_WRITE_DISCARD, 0, &mappedTexture);
    if (FAILED(hr))
        return;

    D3D10_TEXTURE2D_DESC desc;     
    m_pTextureBuffer->GetDesc( &desc );     

   // �����������û�������Tile������Ҫ���հ״���0���Է���һ�ε������ݸ���
   int w = rcSrc.right - rcSrc.left;
   int h = rcSrc.bottom - rcSrc.top;
   if (w != TILE_SIZE || h != TILE_SIZE)
    {
        byte*  pTexels = (byte*)mappedTexture.pData;
        /* for (int y = 0; y < TILE_SIZE; y++)
        {
            memset(pTexels, 0, 4 * TILE_SIZE);
            pTexels += mappedTexture.RowPitch;
        } */
		memset(pTexels, 0, mappedTexture.RowPitch * TILE_SIZE);
    } 

    BYTE*  pSrcBits = (BYTE*)source.pFirstLineBits;
    byte*  pTexels = (byte*)mappedTexture.pData;    

    pSrcBits += rcSrc.top * source.pitch;

    if (source.hasAlphaChannel)
    {
		int w = rcSrc.right - rcSrc.left;

		// ��Ҫʹ��for{forѭ����Ч��̫�͡�   
		for (int row = rcSrc.top; row < rcSrc.bottom; row++)
		{
			memcpy(
				pTexels,
				pSrcBits + (rcSrc.left * 4),
				w * 4);

			/*
			int dstcol = 0;
			for (int col = rcSrc.left; col < rcSrc.right; col++, dstcol++)
			{
				pTexels[dstcol*4 + 0] = pSrcBits[col*4 + 2]; // Red
				pTexels[dstcol*4 + 1] = pSrcBits[col*4 + 1]; // Green
				pTexels[dstcol*4 + 2] = pSrcBits[col*4 + 0]; // Blue
				pTexels[dstcol*4 + 3] = pSrcBits[col*4 + 3]; // Alpha
			}
			*/

			pSrcBits += source.pitch;
			pTexels += mappedTexture.RowPitch;
		}
    }
    else
    {
        for (int row = rcSrc.top; row < rcSrc.bottom; row++)  
        {       
            int dstcol = 0;
            for (int col = rcSrc.left; col < rcSrc.right; col++, dstcol++)  
            {        
                pTexels[dstcol*4 + 0] = pSrcBits[col*4 + 2]; // Red       
                pTexels[dstcol*4 + 1] = pSrcBits[col*4 + 1]; // Green     
                pTexels[dstcol*4 + 2] = pSrcBits[col*4 + 0]; // Blue        
                pTexels[dstcol*4 + 3] = 255;
            }    

            pSrcBits += source.pitch;
            pTexels += mappedTexture.RowPitch;
        }  
    }

    m_pTextureBuffer->Unmap(nSub);



#ifdef _DEBUGx
    static bool bDebug = false;
    if (bDebug)
    {
        static int i = 0; 
        i++;
        TCHAR szTest[256];
        _stprintf(szTest, TEXT("c:\\AAA\\%d.dds"),i);
        D3DX10SaveTextureToFile(m_pTextureBuffer, D3DX10_IFF_DDS, szTest);

        CImage image;
        image.Attach(source.hBitmap);

        _stprintf(szTest, TEXT("c:\\AAA\\%d_(%d_%d_%d_%d).png"),i,
                rcSrc.left,rcSrc.top,rcSrc.right,rcSrc.bottom);            
        image.Save(szTest, Gdiplus::ImageFormatPNG);
        image.Detach();
    }
#endif
}

bool TextureTile::create()
{
    if (m_pTextureBuffer || m_pShaderResourceView)
        return false;

    if (!D3D10App::Get()->m_pDevice)
        return false;

    D3D10_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Width = TILE_SIZE;
    textureDesc.Height = TILE_SIZE;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
	// windows bitmap��RGB����ΪBGRA��˳��
    textureDesc.Format = /*DXGI_FORMAT_R8G8B8A8_UNORM*/ DXGI_FORMAT_B8G8R8A8_UNORM; 
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    textureDesc.Usage = D3D10_USAGE_DYNAMIC;
    textureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;

    HRESULT hr = D3D10App::Get()->m_pDevice->CreateTexture2D(&textureDesc, 
        nullptr, &m_pTextureBuffer);
    if (FAILED(hr))
        return false;

    D3D10_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    hr = D3D10App::Get()->m_pDevice->CreateShaderResourceView(
            m_pTextureBuffer, 
            &shaderResourceViewDesc, 
            &m_pShaderResourceView);
    if (FAILED(hr))
    {
        SAFE_RELEASE(m_pTextureBuffer);
        return false;
    }

    return true;
}


void  TextureTile::SetIndex(int xIndex, int yIndex)
{
    m_xIndex = xIndex;
    m_yIndex = yIndex;
}

// vertexStartIndex:
// �����ÿ��tile��������һ��vertex buffer����ᵼ��ÿ�λ���֮ǰ��Ҫ�л���ǰvertex buffer��
// ʹ����Ⱦ���ܺܲ�����������������������п��vb��������layer�����У�Ȼ��ͨ������������
void  TextureTile::Compositor(
            long xOffset,
            long yOffset,
            long vertexStartIndex,
            UI::GpuLayerCommitContext* pContext)
{
    if (!m_pShaderResourceView)
        return ;

	float pos[2] =
	{
		(float)(pContext->m_xOffset),
		(float)(pContext->m_yOffset)
	};
	Effects::m_pFxVsDestPos->SetFloatVector(pos);

    ID3D10EffectTechnique*  pTech = nullptr;
    if (pContext->m_bTransformValid)
    {
        Effects::m_pFxMatrix->SetMatrix(
			(float*)(D3DXMATRIX*)pContext->m_matrixTransform);

        pTech = Effects::m_pTechDrawTextureMatrix;
    }
    else
    {
        pTech = Effects::m_pTechDrawTexture;
    }

	if (pContext->m_fAlpha != 1.0f)
		Effects::m_pFxAlpha->SetFloat(pContext->m_fAlpha);
    Effects::m_pFxTexture10->SetResource(m_pShaderResourceView);

    D3D10_TECHNIQUE_DESC techDesc;
    pTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        pTech->GetPassByIndex(p)->Apply(0);
        D3D10App::Get()->m_pDevice->Draw(4, vertexStartIndex);
    }

	if (pContext->m_fAlpha != 1.0f)
	{
		Effects::m_pFxAlpha->SetFloat(1.0f);
	}
}


