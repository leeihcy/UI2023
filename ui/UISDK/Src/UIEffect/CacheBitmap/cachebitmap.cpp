#include "stdafx.h"
#include "cachebitmap.h"


CacheBitmap::CacheBitmap()
{
}
CacheBitmap::~CacheBitmap()
{
    Destroy();
}

void  CacheBitmap::Destroy()
{
    if (!m_buffer.IsNull())
    {
        m_buffer.Destroy();
    }
}

HBITMAP  CacheBitmap::Create(int nWidth, int nHeight)
{
    Image* p = CreateEx(nWidth, nHeight);
    if (p)
        return p->GetHBITMAP();

    return nullptr;
}

Image*  CacheBitmap::CreateEx(int nWidthReq, int nHeightReq)
{
    int nWidth = 0; 
    int nHeight = 0;
    AdjustWH(nWidthReq, nHeightReq, nWidth, nHeight);

    if (!m_buffer.IsNull() && 
        (m_buffer.GetHeight() < nHeight || m_buffer.GetWidth() < nWidth))
    {
        nWidth = max(m_buffer.GetWidth(), nWidth);
        nHeight = max(m_buffer.GetHeight(), nHeight);

        Destroy();
    }

    if (m_buffer.IsNull())
    {
        // 负数表示创建一个top-down DIB,这样坐标与位图能够直接对应，便于计算
        m_buffer.Create(nWidth, IsTopdowmBitmap() ? -nHeight:nHeight, 32, Image::createAlphaChannel);  
    }
    else
    {
        HDC hDC = m_buffer.GetDC();
        RECT rc = {0, 0, nWidthReq, nHeightReq};
        ::FillRect(hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
        m_buffer.ReleaseDC();
    }

    return &m_buffer;
}

bool  CacheBitmap::IsTopdowmBitmap()
{
    return true;
}

void  CacheBitmap::Clear(DWORD dwColor, RECT* prc)
{
    if (m_buffer.IsNull() || nullptr == prc)
        return;

    RECT rcDst;
    RECT rcTemp = { 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight() };
    if (!IntersectRect(&rcDst, &rcTemp, prc))
        return;

    byte* pBits = (byte*)m_buffer.GetBits();

    pBits = (pBits + (rcDst.top * m_buffer.GetPitch() + prc->left*4));
    for (int y = rcDst.top; y < rcDst.bottom; y++)
    {
        int* p = (int*)pBits;
        for (int x = rcDst.left; x < rcDst.right; x++, p++)
        {
            *p = dwColor;
        }
        pBits += m_buffer.GetPitch();
    }
}
// 
// void  CacheBitmap::Transparent(DWORD dwTransparent, RECT* prc)
// {
//     if (nullptr == m_hCacheBitmap || nullptr == prc)
//         return;
// 
//     RECT rcDst;
//     RECT rcTemp = { 0, 0, m_nWidth, m_nHeight };
//     if (!IntersectRect(&rcDst, &rcTemp, prc))
//         return;
// 
//     BITMAP  bm;
//     ::GetObject(m_hCacheBitmap, sizeof(bm), &bm);
//     byte* pBits = (byte*)bm.bmBits;
// 
//     pBits = (pBits + (rcDst.top * bm.bmWidthBytes + prc->left*4));
//     int nSize = (rcDst.right-rcDst.left)*4;
//     for (int y = rcDst.top; y < rcDst.bottom; y++)
//     {
//         int* p = (int*)pBits;
//         for (int x = rcDst.left; x < rcDst.right; x++, p++)
//         {
//             if (*p == dwTransparent)
//                 *p = 0;
//         }
//         pBits += bm.bmWidthBytes;
//     }
// }

void  CacheBitmap::AdjustWH(int nWidth, int nHeight, int& nLastWidth, int& nLastHeight)
{
#if 1
    nLastWidth = nWidth;
    nLastHeight = nHeight;
#else
//     nLastWidth = CalcMin2(nWidth);  // 将pitch作成2的n次方，加快定位
//     nLastHeight = nHeight;
#endif
}

// 计算2的平方
int  CacheBitmap::CalcMin2(int n)
{
    int test = 1;
    for (int i = 1; i <= 20; i++)
    {
        test <<= 1;
        if (n <= test)
            return test;
    }

    return n;
}

HDC  CacheBitmap::GetMemDC()
{
    return m_buffer.GetDC();
}
void  CacheBitmap::ReleaseMemDC(HDC)
{
    m_buffer.ReleaseDC();
}
void  CacheBitmap::SetPixel(int x, int y, long color)
{
    m_buffer.SetPixelBit32(x, y, color);
}

