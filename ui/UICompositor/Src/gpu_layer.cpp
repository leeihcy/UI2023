#include "stdafx.h"
#include "gpu_layer.h"
#include "texture_tile.h"
#include "gpu_compositor.h"
#include "D3D10/d3dapp.h"
#include "hard3dtransform.h"
#include "common_def.h"
#include "d3d10\common/RenderStates.h"
#include "d3d10\common/Effects.h"

using namespace ui;


// http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome

GpuRenderLayer::GpuRenderLayer()
{
	m_pVertexBuffer = nullptr;
	m_pParent = m_pChild = m_pNext = nullptr;
	m_size.cx = m_size.cy = 0;

    m_pCompositor = nullptr;

#if 0
	D3DXMatrixIdentity(&m_transform);
	m_bNeedUpdateTransform = false;
    m_bTransformValid = false;

	m_fTranslationX = m_fTranslationY = m_fTranslationZ = 0;
	m_fRotationX = m_fRotationY = m_fRotationZ = 0;
	m_fScaleX = m_fScaleY = m_fScaleZ = 1;
#endif
}

GpuRenderLayer::~GpuRenderLayer()
{
    SAFE_RELEASE(m_pVertexBuffer);

	GpuRenderLayer* pChild = m_pChild;
	while (pChild)
	{
		GpuRenderLayer* pTemp = pChild->m_pNext;
		SAFE_DELETE(pChild);
		pChild = pTemp;
	}
	m_pChild = m_pNext = nullptr;
	m_pParent = nullptr;

    list<TextureTile*>::iterator iter = m_listTile.begin();
    for (; iter != m_listTile.end(); ++iter)
    {
        delete *iter;
    }
    m_listTile.clear();
}

IGpuRenderLayer*  GpuRenderLayer::GetIGpuLayerTexture()
{
    return static_cast<IGpuRenderLayer*>(this);
}

void  GpuRenderLayer::Release()
{
    delete this;
}

void  GpuRenderLayer::SetHardwareComposition(GpuComposition* p)
{
    m_pCompositor = p;
}

//----------------------------------------------------------------------------------------
//
// D3D11_USAGE_DYNAMIC
// Dynamic��Դ��������ʵ��ÿһ֡�н������ݴ�CPU�ϴ���GPU
//
// A resource that is accessible by both the GPU (read only) and the CPU (write only). 
// A dynamic resource is a good choice for a resource that will be updated by the CPU 
// at least once per frame. To update a dynamic resource, use a Map method.
// 
// Dynamic��Դ��ʹ�÷��� ��How to: Use dynamic resources��
// http://msdn.microsoft.com/zh-cn/subscriptions/downloads/dn508285.aspx
//
// ��Դ���� ��D3D11_USAGE enumeration��
// http://msdn.microsoft.com/zh-cn/subscriptions/downloads/ff476259.aspx
//
//---------------------------------------------------------------------------------------
//
// D3D10_MAP_WRITE_DISCARD�Ļ��ƣ�
//
// ��������ԭ�е����ݣ�����һ���µĿջ����CPU��GPU�Ͳ��õȴ�CPU��
// �����Ҳ���ܶ����bufferʹ��������ˣ�����������������buffer. <<--
//    �ֲ�������������»����������֮������ݻ��������ݶ�ʧ��������
//
// --> ���������ʹ�÷ֿ���ƣ������������ڵĿ���¼��ɡ�
//
// To receive a performance improvement when you use dynamic vertex buffers, 
// your app must call Map with the appropriate D3D11_MAP values. c
// D3D11_MAP_WRITE_DISCARD indicates that the app doesn't need to keep the old
// vertex or index data in the buffer. If the GPU is still using the buffer 
// when you call Map with D3D11_MAP_WRITE_DISCARD, the runtime returns a 
// pointer to a new region of memory instead of the old buffer data. This 
// allows the GPU to continue using the old data while the app places data in 
// the new buffer. No additional memory management is required in the app; the
// old buffer is reused or destroyed automatically when the GPU is finished 
// with it.
//
//---------------------------------------------------------------------------------------
//

void  GpuRenderLayer::UploadHBITMAP(UploadGpuBitmapInfo& info)
{
	if (!info.hBitmap)
		return;

	UINT nCount = info.nCount;
	RECT* prcArray = info.prcArray;

	RECT  rcFull = { 0, 0, info.width, info.height };
	if (0 == info.nCount || nullptr == info.prcArray)
	{
		nCount = 1;
		prcArray = &rcFull;
	}

	for (UINT i = 0; i < nCount; i++)
	{
		RECT  rc = rcFull;
		if (prcArray)
			IntersectRect(&rc, &rc, &prcArray[i]);

		upload_bitmap_rect(rc, info);
	}
}

void  GpuRenderLayer::upload_bitmap_rect(RECT& rc, UploadGpuBitmapInfo& source)
{
    // ������Ӱ��� tile 
    // -1: ����(0~128)����Ӱ��ľ���һ������0����Ӱ������1(128/128=1)
    int xIndexFrom = rc.left / TILE_SIZE;
    int xIndexTo = (rc.right-1) / TILE_SIZE;
    int yIndexFrom = rc.top / TILE_SIZE;
    int yIndexTo = (rc.bottom-1) / TILE_SIZE;

#if ENABLE_TRACE
    char szText[32];
    sprintf(szText, "Upload2Gpu: %d,%d  %d,%d\r\n",
        rc.left,
        rc.top,
        rc.right-rc.left,
        rc.bottom-rc.top);
    OutputDebugStringA(szText);
#endif

    RECT rcSrc;
    for (int y = yIndexFrom; y <= yIndexTo; y++)
    {
        for (int x = xIndexFrom; x <= xIndexTo; x++)
        {
            rcSrc.left = x*TILE_SIZE;
            rcSrc.top = y*TILE_SIZE;
            rcSrc.right = rcSrc.left + TILE_SIZE;
            rcSrc.bottom = rcSrc.top + TILE_SIZE;

            // ���������С������ֱ�Ӻ��ԡ���һ���������λ���Ҳ�͵ײ���
            // ��Щ�ֿ�
            if (rcSrc.right > m_size.cx)
                rcSrc.right = m_size.cx;
            if (rcSrc.bottom > m_size.cy)
                rcSrc.bottom = m_size.cy;

            m_arrayTile[y][x]->Upload(rcSrc, source);

#if ENABLE_TRACE
            char szText[32];
            sprintf(szText, "Update Tile: (%d,%d)\r\n", x, y);
            OutputDebugStringA(szText);
#endif
        }
    }
}

//
//  ע������ÿ���ֿ鶼���õ��ǹ̶���С��λ�����Ҳ�͵ײ��ķֿ�Ҳ��һ����
//      ��С��������ȥƥ����ʵ�Ĵ��ڴ�С��
//      �������ĺô��Ǵ��ڸı��Сʱ����Ƶ����ɾ������·ֿ顣
//
void  GpuRenderLayer::create_tile(ULONG row, ULONG col)
{
    assert (row > 0 && col > 0);

    if (m_arrayTile.GetCol() == col && 
        m_arrayTile.GetRow() == row)
    {
        return;
    }

    int nCountNew = row * col;
    int nCountNow = m_arrayTile.GetCol() * m_arrayTile.GetRow();

    // ��������
    long lDiff = nCountNew - nCountNow;
    if (lDiff > 0)
    {
        // ���
        for (int i = 0; i < lDiff; i++)
        {
            m_listTile.push_back(new TextureTile);
        }

#if ENABLE_TRACE
        char szText[32];
        sprintf(szText, "alloc tiles: %d\r\n", lDiff);
        OutputDebugStringA(szText);
#endif
    }
    else if (lDiff < 0)
    {
        // ɾ��
        list<TextureTile*>::iterator iter = m_listTile.begin();
        for (int i = 0; i < lDiff; i++)
        {
            delete *iter;
            iter = m_listTile.erase(iter);
        }

#if ENABLE_TRACE
        char szText[32];
        sprintf(szText, "release tiles: %d\r\n", lDiff);
        OutputDebugStringA(szText);
#endif 
    }

    // ����λ�õ���
    m_arrayTile.Create(row, col);

#if ENABLE_TRACE
    char szText[32];
    sprintf(szText, "tile row=%d, col=%d\r\n", row, col);
    OutputDebugStringA(szText);
#endif 

    list<TextureTile*>::iterator iter = m_listTile.begin();
    for (ULONG y = 0; y < row; y++)
    {
        for (ULONG x = 0; x < col; x++)
        {
            m_arrayTile[y][x] = (*iter);
            (*iter)->SetIndex(x, y);
            ++iter;
        }
    }
}

void  GpuRenderLayer::Resize(UINT nWidth, UINT nHeight)
{
    if (m_size.cx == (int)nWidth && 
        m_size.cy == (int)nHeight)
    {
        return;
    }

    int col = (int)ceil((float)nWidth / TILE_SIZE);
    int row = (int)ceil((float)nHeight / TILE_SIZE);

    create_tile(row, col);

    m_size.cx = nWidth;
    m_size.cy = nHeight;

    // ����vbo
    SAFE_RELEASE(m_pVertexBuffer);
    
    D3D10_BUFFER_DESC BufDesc;
    BufDesc.ByteWidth = sizeof(DXUT_SCREEN_VERTEX_10) * 4 * row * col;
    BufDesc.Usage = D3D10_USAGE_DYNAMIC;
    BufDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    BufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    BufDesc.MiscFlags = 0;

    D3D10App::Get()->m_pDevice->CreateBuffer(&BufDesc,
        nullptr,
        &m_pVertexBuffer);

    // �ϴ�һ��������Ҫ�õĶ������ݡ�������
    
    DXUT_SCREEN_VERTEX_10* pVB = nullptr;
    if (SUCCEEDED(m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (LPVOID*)&pVB)))
    {
        for (int y = 0; y < row; ++y)
        {
            for (int x = 0; x < col; ++x)
            {
                int x1 = x + 1;
                int y1 = y + 1;

                UI::D3DCOLORVALUE color = { 1, 1, 1, 1 };
                DXUT_SCREEN_VERTEX_10 vertices[4] =
                {
                    { (float)TILE_SIZE*x,  (float)TILE_SIZE*y,  0.f, color, 0, 0 },
                    { (float)TILE_SIZE*x1, (float)TILE_SIZE*y,  0.f, color, 1, 0 },
                    { (float)TILE_SIZE*x,  (float)TILE_SIZE*y1, 0.f, color, 0, 1 },
                    { (float)TILE_SIZE*x1, (float)TILE_SIZE*y1, 0.f, color, 1, 1 },
                };

                CopyMemory(pVB, vertices, sizeof(DXUT_SCREEN_VERTEX_10) * 4);
                pVB += 4;
            }
        }
       
        m_pVertexBuffer->Unmap();
    }
}

void  GpuRenderLayer::CalcDrawDestRect(__in RECTF* prc, __out RECTF* prcfOut)
{
    prcfOut->left = prc->left * 2.0f / m_size.cx - 1.0f;
    prcfOut->right = prc->right * 2.0f / m_size.cx - 1.0f;
    prcfOut->top = 1.0f - prc->top * 2.0f / m_size.cy;
    prcfOut->bottom = 1.0f - prc->bottom * 2.0f / m_size.cy;
}
void  GpuRenderLayer::CalcDrawDestRect(int xDest, int yDest, UINT wDest, UINT hDest, __out RECTF* prcfOut)
{
    prcfOut->left = xDest * 2.0f / m_size.cx - 1.0f;
    prcfOut->right = (xDest+wDest) * 2.0f / m_size.cx - 1.0f;
    prcfOut->top = 1.0f - yDest * 2.0f / m_size.cy;
    prcfOut->bottom = 1.0f - (yDest+hDest) * 2.0f / m_size.cy;
}
void  GpuRenderLayer::CalcDrawDestRect(float xDest, float yDest, float wDest, float hDest, __out RECTF* prcfOut)
{
    prcfOut->left = xDest * 2.0f / m_size.cx - 1.0f;
    prcfOut->right = (xDest+wDest) * 2.0f / m_size.cx - 1.0f;
    prcfOut->top = 1.0f - yDest * 2.0f / m_size.cy;
    prcfOut->bottom = 1.0f - (yDest+hDest) * 2.0f / m_size.cy;
}

void  MultiMatrix(GpuLayerCommitContext& c, float* matrix16);

void  GpuRenderLayer::Compositor(GpuLayerCommitContext* pContext, float* pMatrixTransform)
{
    if (0 == m_size.cx || 0 == m_size.cy)
        return;
	if (!pContext)
		return;

//     if (pTransform && pTransform->get_type() != TRANSFORM_HARD3D)
//         return;

    ID3D10Device*  pDevice = D3D10App::Get()->m_pDevice;

    // ����stencilӦ�����ø�����ľ��󣬶���Ӧ
    // �ð����Լ�����ת����������context�ｫ������ľ������������
    if (pContext->m_bTransformValid)
    {
        // ������о���任�������������ܲ�����һ�����Σ���ʱӦ�ò���
        // ģ�建��ķ�ʽ�����м���

        // 1. ��ԭscissor����Ϊ������Ļ
        SIZE sizeWnd = m_pCompositor->GetSize();
        D3D10_RECT rects;
        SetRect((LPRECT)&rects, 0, 0, sizeWnd.cx, sizeWnd.cy);
        pDevice->RSSetScissorRects(1, &rects);

        // 2. �����ǰģ�建��
        m_pCompositor->ClearStencil();

        // 3. ��ֹback bufferд��
        ID3D10BlendState* pOldBlendState = nullptr;
        pDevice->OMGetBlendState(&pOldBlendState, 0, 0);
        pDevice->OMSetBlendState(
            RenderStates::pBlendStateDisableWriteRenderTarget, 0, 0xFFFFFFFF);

        // 4. ��pContext->m_rcClip���ģ�建��Ϊ1
        pDevice->OMSetDepthStencilState(RenderStates::pStencilStateCreateClip, 1);

        Effects::m_pFxMatrix->SetMatrix((float*)pContext->m_matrixTransform);
        RECTF rcTextArg = {0,0, 1,1};
        RECTF rcDraw;
        rcDraw.left   = (float)pContext->m_rcClip.left;
        rcDraw.top    = (float)pContext->m_rcClip.top;
        rcDraw.right  = (float)pContext->m_rcClip.right;
        rcDraw.bottom = (float)pContext->m_rcClip.bottom;
        D3D10App::Get()->ApplyTechnique(Effects::m_pTechFillRectMatrix, 
            &rcDraw, &rcTextArg, 1);

        // 5. �ָ�back bufferд��
        pDevice->OMSetBlendState(pOldBlendState, 0, 0xFFFFFFFF);
        SAFE_RELEASE(pOldBlendState);

        // 6. ����ģ�建�溯����ֻ����ǰģ��ֵΪ1��λ��ͨ�����ԣ����ã�
        pDevice->OMSetDepthStencilState(RenderStates::pStencilStateClip, 1);
    }
    else
    {
        D3D10_RECT rects[1];
        memcpy(rects, &pContext->m_rcClip, sizeof(RECT));
        pDevice->RSSetScissorRects(1, rects);
    }

	if (pMatrixTransform)
	{
		MultiMatrix(*pContext, pMatrixTransform);
	}

    UINT stride = sizeof(DXUT_SCREEN_VERTEX_10);
    UINT offset = 0;
    D3D10App::Get()->m_pDevice->IASetVertexBuffers(0, 1,
        &m_pVertexBuffer,
        &stride, &offset);

    ULONG row = m_arrayTile.GetRow();
    ULONG col = m_arrayTile.GetCol();

    long xOffset = 0;
    long yOffset = 0;
    for (ULONG y = 0; y < row; y++)
    {
        yOffset = y * TILE_SIZE;
        for (ULONG x = 0; x < col; x++)
        {
            xOffset = x * TILE_SIZE;
            
            m_arrayTile[y][x]->Compositor(xOffset, yOffset, (y*col + x) * 4, pContext);
        }
    }

    if (pContext->m_bTransformValid)
    {
        // 7. �ر�ģ�建��
        pDevice->OMSetDepthStencilState(
             RenderStates::pStencilStateDisable, 0);
    }
}

// void   GpuRenderLayer::oBitBlt(int xDest, int yDest, int /*wDest*/, int /*hDest*/, int xSrc, int ySrc)
// {
//     oStretchBlt((float)xDest, (float)yDest, (float)m_size.cx, (float)m_size.cy, xSrc, ySrc, m_size.cx, m_size.cy);
// }

#if 0
void  GpuLayerTexture::CalcTransform()
{
	if (!m_bNeedUpdateTransform)
		return;

	m_bNeedUpdateTransform = false;


    D3DXMATRIX  temp;
	D3DXMatrixIdentity(&m_transform);

	D3DXMatrixScaling(&m_transform, m_fScaleX, m_fScaleY, m_fScaleZ);

    // ��λ����
	D3DXMatrixRotationYawPitchRoll(&temp, m_fRotationX, m_fRotationY, m_fRotationZ);
    m_transform *= temp;

	D3DXMatrixTranslation(&temp, m_fTranslationX, m_fTranslationY, m_fTranslationZ);
    m_transform *= temp;

    D3DXMATRIX  identify;
    D3DXMatrixIdentity(&identify);
    if (m_transform == identify)
    {
        m_bTransformValid = false;
    }
    else
    {
        m_bTransformValid = true;
    }
}


void  GpuLayerTexture::Translation(float xPos, float yPos, float zPos)
{
//     GpuLayerTexture* pRoot = m_pCompositor->GetRootLayerTexture();
//     if (!pRoot)
//         return;
// 
//     if (0 == pRoot->m_size.cx || 0 == pRoot->m_size.cy)
//         return;
// 
// 	   m_fTranslationZ = zPos;  // TODO:
// 
//     m_fTranslationX = xPos * 2.0f / pRoot->m_size.cx - 1.0f;
//     m_fTranslationY = 1.0f - yPos * 2.0f / pRoot->m_size.cy;

    m_fTranslationX = xPos;
    m_fTranslationY = yPos;
    m_fTranslationZ = zPos;
	m_bNeedUpdateTransform = true;

// 	TCHAR szText[32] = {0};
// 	wprintf(szText, TEXT("%f\n"), yPos);
// 	::OutputDebugString(szText);
}
void  GpuLayerTexture::TranslationBy(float xPos, float yPos, float zPos)
{
	m_fTranslationX += xPos;
	m_fTranslationY += yPos;
	m_fTranslationZ += zPos;

	m_bNeedUpdateTransform = true;
}

void  GpuLayerTexture::Rotate(float xRotate, float yRotate, float zRotate)
{
    m_fRotationX = xRotate;
    m_fRotationY = yRotate;
    m_fRotationZ = zRotate;

    m_bNeedUpdateTransform = true;
}
void  GpuLayerTexture::RotateBy(float xRotate, float yRotate, float zRotate)
{
    m_fRotationX += xRotate;
    m_fRotationY += yRotate;
    m_fRotationZ += zRotate;

    m_bNeedUpdateTransform = true;
}

void  GpuLayerTexture::Scale(float xScale, float yScale, float zScale)
{
    m_fScaleX = xScale;
    m_fScaleY = yScale;
    m_fScaleZ = zScale;

    m_bNeedUpdateTransform = true;
}
void  GpuLayerTexture::ScaleBy(float xScale, float yScale, float zScale)
{
    m_fScaleX += xScale;
    m_fScaleY += yScale;
    m_fScaleZ += zScale;

    m_bNeedUpdateTransform = true;
}
#endif