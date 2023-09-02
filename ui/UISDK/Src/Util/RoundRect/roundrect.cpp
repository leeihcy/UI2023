#include "stdafx.h"
#include "src/Atl\image.h"
#include "roundrect.h"
#include <assert.h>


using namespace ui;

// TODO: 如何优化除法处理

RoundRectHandler::RoundRectHandler()
{
    lefttop = leftbottom = righttop = rightbottom = 0;

    m_hBitmap = nullptr;
    m_hDC = nullptr;
    m_nPitch = 0;
    m_pBits = nullptr;
    m_nBitmapWidth = m_nBitmapHeigth = 0;
    m_nBitmapAvailableWidth = m_nBitmapAvailableHeight = 0;
//    m_bHandleBorder = true;
}
RoundRectHandler::~RoundRectHandler()
{
    DestroyBuffer();
    if (m_hDC)
        DeleteDC(m_hDC);
}
//
// 将TOP 、BOTTOM 这两行虚化，与圆角进行衔接
//
// pDstBits： 这一行开始的字节（0,0）或者（0, bottom）
//
// void RoundRectHandler::HandleXLine(byte* pDstBits, int xFrom, int xTo)
// {
//     int i = 0;
//     for (int x = xFrom; x < xTo; x++)
//     {
//         i = x<<2;
//         pDstBits[i++] = pDstBits[i]>>1;
//         pDstBits[i++] = pDstBits[i]>>1;
//         pDstBits[i++] = pDstBits[i]>>1;
//         pDstBits[i]   = pDstBits[i]>>1;
//     }
// }
//
// 将LEFT、RIGHT 这两列进行虚化，与圆角进行衔接
// 
// pDstBits：这一列开始的位置（0,0）或者（right, 0）
// void RoundRectHandler::HandleYLine(byte* pDstBits, int nDstPitch, int yFrom, int yTo)
// {
//     pDstBits = pDstBits + (yFrom)*nDstPitch;
//     for (int y = yFrom; y < yTo; y++)
//     {
//         pDstBits[0] = pDstBits[0]>>1;
//         pDstBits[1] = pDstBits[1]>>1;
//         pDstBits[2] = pDstBits[2]>>1;
//         pDstBits[3] = pDstBits[3]>>1;
// 
//         pDstBits += nDstPitch;
//     }
// }


void  RoundRectHandler::HandleRightTopCorner(CORNER_DATA& data)
{
	byte* pDstBits = data.pDstBits + (data.nDstPitch)*data.yDst;
	byte* pSrcBits = data.pSrcBits + (data.nSrcPitch)*data.ySrc;
	

    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

	int yLoop = data.height;

    int xLoop = 0;
	int xDelta = 1;
	int xBegin = data.width-1;
	if (data.bClipMode)
	{
		xLoop = 0;
		xDelta = 0;
	}

    for (int y = 0; y < yLoop; y++)
    {
        for (int x = xBegin; x >= xLoop; x--)
        {
            i = (data.xDst+x)<<2;
            a = pSrcBits[((data.xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }

		xLoop += xDelta;

		pDstBits += data.nDstPitch;
		pSrcBits += data.nSrcPitch;
    }
}

void  RoundRectHandler::HandleRightBottomCorner(CORNER_DATA& data)
{
	byte* pDstBits = data.pDstBits + (data.nDstPitch)*data.yDst;
	byte* pSrcBits = data.pSrcBits + (data.nSrcPitch)*data.ySrc;
	
    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

	int yLoop = data.height;

    int xLoop = data.width-1;
	int xBegin = data.width-1;
	int xDelta = 1;

	if (data.bClipMode)
	{
		xLoop = 0;
		xDelta = 0;
	}
	
    for (int y = 0; y < yLoop; y++)
    {
        for (int x = xBegin; x >= xLoop; x--)
        {
            i = (data.xDst+x)<<2;
            a = pSrcBits[((data.xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }

		xLoop -= xDelta;

		pDstBits += data.nDstPitch;
		pSrcBits += data.nSrcPitch;
    }
}


void  RoundRectHandler::HandleLeftBottomCorner(CORNER_DATA& data)
{
	byte* pDstBits = data.pDstBits + (data.nDstPitch)*data.yDst;
	byte* pSrcBits = data.pSrcBits + (data.nSrcPitch)*data.ySrc;

    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

	int yLoop = data.height;

    int xLoop = 0;
	int xDelta = 1;
	if (data.bClipMode)
	{
		xDelta = 0;
		xLoop = data.width;
	}

    for (int y = 0; y < yLoop; y++)
    {
        for (int x = 0; x <= xLoop; x++)
        {
            i = (data.xDst+x)<<2;
            a = pSrcBits[((data.xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }

		xLoop += xDelta;

        pDstBits += data.nDstPitch;
        pSrcBits += data.nSrcPitch;
    }
}

void  RoundRectHandler::HandleLeftTopCorner(CORNER_DATA& data)
{
	byte* pDstBits = data.pDstBits + (data.nDstPitch)*data.yDst;
	byte* pSrcBits = data.pSrcBits + (data.nSrcPitch)*data.ySrc;

    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha 
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

	int yLoop = data.height;
	int xLoop = data.width;

	int xDelta = 1;
	if (data.bClipMode)
	{
		xDelta = 0;
	}

    for (int y = 0; y < yLoop; y++)
    {
        for (int x = 0; x < xLoop; x++)
        {
            i = (data.xDst+x)<<2;
            a = pSrcBits[((data.xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }

		xLoop -= xDelta;

        pDstBits += data.nDstPitch;
        pSrcBits += data.nSrcPitch;
    }
}


// 注：hBitmap为带预乘结果的位图数据(Gdiplus的结果是带预乘的)
bool  RoundRectHandler::Apply2Bitmap(
        HBITMAP hBitmap, 
        bool bTopDown, 
        RECT* prcRegion,
		RECT* prcClip)
{
    if (!hBitmap || !m_hBitmap)
        return false;

    DIBSECTION  dib = {0};
    if (sizeof(dib) != GetObject(hBitmap, sizeof(dib), &dib))
        return false;

    BITMAP& bm = dib.dsBm;

    CRect rcDest(0, 0, bm.bmWidth, bm.bmHeight);
    if (prcRegion)
    {
        ::IntersectRect(&rcDest, prcRegion, &rcDest);
    }

    if (rcDest.Width() < (int)GetMinWidth() || 
        rcDest.Height() < (int)GetMinHeight())
        return false;

    byte*  pDst = (byte*)bm.bmBits;
    int    nDstPitch = bm.bmWidthBytes;
    if (!bTopDown)
    {
        pDst = pDst + (bm.bmHeight-1)*nDstPitch;
        nDstPitch = -nDstPitch;
    }
    byte*  pDstBits = pDst;
    byte*  pSrcBits = m_pBits;

	CORNER_DATA data = {0};
	data.pDstBits = pDstBits;
	data.pSrcBits = pSrcBits;
	data.nDstPitch = nDstPitch;
	data.nSrcPitch = m_nPitch;
	data.bClipMode = false;

    if (lefttop)
    {
		data.xDst = rcDest.left;
		data.yDst = rcDest.top;
		data.xSrc = 0;
		data.ySrc = 0;
		data.width = data.height = lefttop;

		if (prcClip)
		{
			RECT rcRealDst = {
				data.xDst, 
				data.yDst, 
				data.xDst+data.width,
				data.yDst+data.height
			};

			if (IntersectRect(&rcRealDst, &rcRealDst, prcClip))
			{
				data.bClipMode = 1;

				// 源掩图位置也进行等量偏移
				data.xSrc += rcRealDst.left - data.xDst;
				data.ySrc += rcRealDst.top - data.yDst;

				// 更新剪裁后的参数
				data.xDst = rcRealDst.left;
				data.yDst = rcRealDst.top;
				data.width = rcRealDst.right-rcRealDst.left;
				data.height = rcRealDst.bottom-rcRealDst.top;

				HandleLeftTopCorner(data);
			}
		}
		else
		{
			
			HandleLeftTopCorner(data);
		}
    }

    if (leftbottom)
    {
		data.xDst = rcDest.left;
		data.yDst = rcDest.bottom - leftbottom;
		data.xSrc = 0;
		data.ySrc = m_nBitmapAvailableHeight-leftbottom;
		data.width = data.height = leftbottom;

		if (prcClip)
		{
			RECT rcRealDst = {
				data.xDst, 
				data.yDst, 
				data.xDst+data.width,
				data.yDst+data.height
			};

			if (IntersectRect(&rcRealDst, &rcRealDst, prcClip))
			{
				data.bClipMode = 1;
			
				// 源掩图位置也进行等量偏移
				data.xSrc += rcRealDst.left - data.xDst;
				data.ySrc += rcRealDst.top - data.yDst;

				// 更新剪裁后的参数
				data.xDst = rcRealDst.left;
				data.yDst = rcRealDst.top;
				data.width = rcRealDst.right-rcRealDst.left;
				data.height = rcRealDst.bottom-rcRealDst.top;

				HandleLeftBottomCorner(data);
			}
		}
		else
		{
			HandleLeftBottomCorner(data);
		}
    }

    if (righttop)
    {
		data.xDst = rcDest.right - righttop;
		data.yDst = rcDest.top;
		data.xSrc = m_nBitmapAvailableWidth - righttop;
		data.ySrc = 0;
		data.width = data.height = righttop;

		if (prcClip)
		{
			RECT rcRealDst = {
				data.xDst, 
				data.yDst, 
				data.xDst+data.width,
				data.yDst+data.height
			};

			if (IntersectRect(&rcRealDst, &rcRealDst, prcClip))
			{
				data.bClipMode = 1;

				// 源掩图位置也进行等量偏移
				data.xSrc += rcRealDst.left - data.xDst;
				data.ySrc += rcRealDst.top - data.yDst;

				// 更新剪裁后的参数
				data.xDst = rcRealDst.left;
				data.yDst = rcRealDst.top;
				data.width = rcRealDst.right-rcRealDst.left;
				data.height = rcRealDst.bottom-rcRealDst.top;

				HandleRightTopCorner(data);
			}
		}
		else
		{
			HandleRightTopCorner(data);
		}
    }

    if (rightbottom)
    {
		data.xDst = rcDest.right - rightbottom;
		data.yDst = rcDest.bottom - rightbottom;
		data.xSrc = m_nBitmapAvailableWidth - rightbottom;
		data.ySrc = m_nBitmapAvailableHeight-rightbottom;
		data.width = data.height = rightbottom;

		if (prcClip)
		{
			RECT rcRealDst = {
				data.xDst, 
				data.yDst, 
				data.xDst+data.width,
				data.yDst+data.height
			};

			if (IntersectRect(&rcRealDst, &rcRealDst, prcClip))
			{
				data.bClipMode = 1;

				// 源掩图位置也进行等量偏移
				data.xSrc += rcRealDst.left - data.xDst;
				data.ySrc += rcRealDst.top - data.yDst;

				// 更新剪裁后的参数
				data.xDst = rcRealDst.left;
				data.yDst = rcRealDst.top;
				data.width = rcRealDst.right-rcRealDst.left;
				data.height = rcRealDst.bottom-rcRealDst.top;
				
				HandleRightBottomCorner(data);
			}
		}
		else
		{
			HandleRightBottomCorner(data);
		}
    }


#ifdef _DEBUG
//     static bool bDebugModexxxxxxxxxxxx = false;
//     if (bDebugModexxxxxxxxxxxx)
//     {
//         CImage image;
//         image.Attach(hBitmap);
//         image.Save(L"C:\\aaa.png", ImageFormatPNG);
//         image.Detach();
//     }
#endif
    return true;
}

void  RoundRectHandler::DestroyBuffer()
{
    if (m_hBitmap)
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;

        m_nBitmapWidth = 0;
        m_nBitmapHeigth = 0;
        m_nBitmapAvailableWidth = 0;
        m_nBitmapAvailableHeight = 0;
        m_nPitch = 0;
        m_pBits = nullptr;
    }
}

void  RoundRectHandler::RequestBuffer(unsigned int nWidth, unsigned int nHeight)
{
    if (nWidth > m_nBitmapWidth || nHeight > m_nBitmapHeigth)
    {
        DestroyBuffer();
    }

    if (nullptr == m_hDC)
    {
        m_hDC = CreateCompatibleDC(nullptr);
    }

    if (nullptr == m_hBitmap)
    {
        m_nBitmapWidth = nWidth;
        m_nBitmapHeigth = nHeight;

        Image image;
        image.Create(m_nBitmapWidth, -(int)m_nBitmapHeigth, 32, Image::createAlphaChannel);
        m_nBitmapAvailableWidth = m_nBitmapWidth;
        m_nBitmapAvailableHeight = m_nBitmapHeigth;

        m_pBits = (byte*)image.GetBits();
        m_nPitch = image.GetPitch();
        m_hBitmap = image.Detach();

        SelectObject(m_hDC, m_hBitmap);
    }
    else
    {
        RECT  rc = {0, 0, nWidth, nHeight};
        FillRect(m_hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

        m_nBitmapAvailableWidth = nWidth;
        m_nBitmapAvailableHeight = nHeight;
    }
}

void  RoundRectHandler::CreateMaskBitmap()
{
    int nMinWidth = GetMinWidth();
    int nMinHeight = GetMinHeight();
    if (nMinHeight == 0 || nMinHeight == 0)
        return;

    RequestBuffer(nMinWidth, nMinHeight);

    Gdiplus::GraphicsPath path;
    CreatePath(&path);

    Gdiplus::Graphics g(m_hDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    Gdiplus::SolidBrush brush((Gdiplus::ARGB)Gdiplus::Color::White);
    g.FillPath(&brush, &path);

//     Pen pen(Color::White);
//     g.DrawPath(&pen, &path);

#ifdef _DEBUG
//     static bool bDebugModexxxxxxxxxxxx = false;
//     if (bDebugModexxxxxxxxxxxx)
//     {
//         CImage image;
//         image.Attach(m_hBitmap);
//         image.Save(L"C:\\aaamask.png", ImageFormatPNG);
//         image.Detach();
//     }
#endif
}

//  圆形
inline bool  IsCircle(int nRadiusLT, int nRadiusRT, int nRadiusLB, int nRadiusRB)
{
    if (nRadiusLT == nRadiusLB && 
        nRadiusLT == nRadiusRT &&
        nRadiusRT == nRadiusRB && 
        nRadiusLT != 0)
        return true;

    return false;
}

void  RoundRectHandler::CreatePath(Gdiplus::GraphicsPath* pPath)
{
    if (IsCircle(lefttop, righttop, rightbottom, leftbottom))
    {
        pPath->AddEllipse(0, 0, lefttop*2, lefttop*2);
        return;  
    }

    int width = GetMinWidth();
    int height = GetMinHeight();

    RECT rc = {0, 0, width, height};
    CreateRoundRectPath3(pPath, &rc, lefttop, righttop, leftbottom, rightbottom);
} 

unsigned int  RoundRectHandler::GetMinWidth()
{
    return max((lefttop + righttop), (leftbottom+rightbottom));
}
unsigned int  RoundRectHandler::GetMinHeight()
{
    return max((lefttop + leftbottom), (righttop+rightbottom));
}
void RoundRectHandler::Set(int n)
{
    if (lefttop == n && leftbottom == n &&
        righttop == n && rightbottom == n)
    {
        return;
    }

    lefttop = leftbottom = righttop = rightbottom = n;
    CreateMaskBitmap();
}

void  RoundRectHandler::Set(REGION4* r)
{
    this->Set(r->left,
        r->top, 
        r->right,
        r->bottom);
}

void RoundRectHandler::Set(int nLeftTop, int nRightTop, int nLeftBottom, int nRightBottom)
{
    if (lefttop == nLeftTop && leftbottom == nLeftBottom &&
        righttop == righttop && rightbottom == nRightBottom)
    {
        return;
    }

    lefttop = nLeftTop;
    leftbottom = nLeftBottom;
    righttop = nRightTop;
    rightbottom = nRightBottom;
    CreateMaskBitmap();
}

namespace ui
{
RoundRectHandler  g_CommonRoundRectHandler;
RoundRectHandler  g_Circle24_24RoundRectHandler;


// 构造一个 左半圆 +　右半圆 的圆角矩形区域
//
// 需要配合使用：
// g.SetSmoothingMode(SmoothingModeHighQuality);
// g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
//
bool  CreateRoundRectPath2(Gdiplus::GraphicsPath* pPath, RECT* prc)
{
    if (!pPath || !prc)
        return false;

    // 16px的radius，画出31px的圆角区域，再经过抗锯齿变成32
    int width = prc->right - prc->left;
    int height = prc->bottom - prc->top;
    int nRadius = height >> 1;
    int nRadius2 = nRadius*2;

    // [注]如果不开启抗锯齿，这里应该用269(不是270)才能得到一个上下对称的形状
    pPath->AddArc(prc->left + width - nRadius2, prc->top, nRadius2, nRadius2, 270/*-1*/, 180);   
    pPath->AddArc(prc->left, prc->top, nRadius2, nRadius2, 90, 180);       

    // 将两个半圆连起来
    pPath->CloseFigure();    
    return true;
}

// 构造一个圆角矩形，四个圆角的半径为nRadius
bool  CreateRoundRectPath(Gdiplus::GraphicsPath* pPath, RECT* prc, int nRadius)
{
    if (!pPath || !prc)
        return false;

    int nRadius2 = nRadius*2;

    // 右上
    pPath->AddArc(prc->right - nRadius2, prc->top, nRadius2, nRadius2, 270, 90 );   
    // 右下
    pPath->AddArc(prc->right - nRadius2, prc->bottom - nRadius2, nRadius2, nRadius2, 0, 90 );  
    // 左下
    pPath->AddArc(prc->left, prc->bottom - nRadius2, nRadius2, nRadius2, 90, 90 );        
    // 左上
    pPath->AddArc(prc->left, prc->top, nRadius2, nRadius2, 180, 90);       

    pPath->CloseFigure();    
    return true;
}

bool  CreateRoundRectPath4(Gdiplus::GraphicsPath* pPath, RECT* prc, REGION4* pRadius)
{
    return CreateRoundRectPath3(pPath, prc, 
        pRadius->left, pRadius->right, pRadius->right, pRadius->bottom);
}

void  PathAddPoint(Gdiplus::GraphicsPath* pPath, int x, int y)
{
    Gdiplus::Point p;
    p.X = x;
    p.Y = y;
    pPath->AddLines(&p, 1);
}

bool  CreateRoundRectPath3(Gdiplus::GraphicsPath* pPath, RECT* prc, 
    int nRadiusLT, int nRadiusRT, int nRadiusRB, int nRadiusLB)
{
    // 注: arc使用直径
    int lt = nRadiusLT*2;
    int rt = nRadiusRT*2;
    int lb = nRadiusLB*2;
    int rb = nRadiusRB*2;

    // 右上
    if (rt)
        pPath->AddArc(prc->right - rt, prc->top, rt, rt, 270, 90 );   
    else
        PathAddPoint(pPath, prc->right, prc->top);

    // 右下
    if (rb)
        pPath->AddArc(prc->right - rb, prc->bottom - rb, rb, rb, 0, 90 );   
    else
        PathAddPoint(pPath, prc->right, prc->bottom);

    // 左下
    if (lb)
        pPath->AddArc(prc->left, prc->bottom - lb, lb, lb, 90, 90 );    
    else
        PathAddPoint(pPath, prc->left, prc->bottom);

    // 左上
    if (lt)
        pPath->AddArc(prc->left, prc->top, lt, lt, 180, 90 );            
    else
        PathAddPoint(pPath, prc->left, prc->top);

    pPath->CloseFigure(); 
    return true;
}




void  CreateRoundRectPortraitByRadius(__in Gdiplus::Bitmap* pBitmap, int nPortraitSize, int nRadius, __out Image& image)
{
	image.Destroy();
	if (!pBitmap || 0 == nPortraitSize)
		return;

	image.Create(nPortraitSize, -nPortraitSize, 32, 
		Image::createAlphaChannel);

	HDC hDC = image.GetDC();
	{
		Gdiplus::Graphics g(hDC);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);   // <-- 必须加上这个，否则画出来的图片边缘不正确

		Gdiplus::RectF dst(0, 0, (Gdiplus::REAL)nPortraitSize, (Gdiplus::REAL)nPortraitSize);
		g.DrawImage(pBitmap, dst, 0, 0,
			(Gdiplus::REAL)pBitmap->GetWidth(),
			(Gdiplus::REAL)pBitmap->GetHeight(), Gdiplus::UnitPixel);
	}
	image.ReleaseDC();

	g_CommonRoundRectHandler.Set(nRadius);
	g_CommonRoundRectHandler.Apply2Bitmap(image, true, nullptr, nullptr);
}

extern "C"
HBITMAP UIAPI CreateRoundRectImage(
	__in Gdiplus::Bitmap* pBitmap, int nPortraitSize, int nRadius)
{
	Image image;
	CreateRoundRectPortraitByRadius(pBitmap, nPortraitSize, nRadius, image);
	return (HBITMAP)image.Detach();
}


}


