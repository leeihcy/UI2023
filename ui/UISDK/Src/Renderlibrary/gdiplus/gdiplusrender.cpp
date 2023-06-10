#include "stdafx.h"
#include "gdiplusrender.h"
#include "UISDK\Kernel\Src\UIEffect\blur\webkit\shadowblur.h"
#include "UISDK\Kernel\Src\UIEffect\CacheBitmap\cachebitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdirender.h"
#include "UISDK\Kernel\Src\Renderlibrary\renderbuffer.h"
#include "UISDK\Kernel\Src\3dlib\quad_perspective_texture_mapping.h"

GdiplusRenderTarget::GdiplusRenderTarget()
{
// 	m_pGraphics = NULL;
// 	m_hRgnMeta = NULL;
//  m_pGdiMemBitmap = NULL;
    m_hBindDC = NULL;
    m_lDrawingRef = 0;
    m_pRenderBuffer = 0;
	m_ptOffset.x = m_ptOffset.y = 0;
}
GdiplusRenderTarget::~GdiplusRenderTarget()
{
// 	SAFE_DELETE(m_pGraphics);
// 	SAFE_DELETE_GDIOBJECT(m_hRgnMeta);
    SAFE_DELETE(m_pRenderBuffer);
	m_hBindDC = NULL;
}
void  GdiplusRenderTarget::Release()
{
    delete this;
}

#if 0  // 效率太低，废弃
//
//	Remark
//		调用Graphics的GetHDC，获取到的HDC不会继承Graphics的任何属性，因此需要我们自己再设置一次属性
//		由于FONT、text color属性移到DrawString中当场设置，因此这两个属性没有设置
//
//		MSDN:
//			Any state changes you make to the device context between GetHDC and ReleaseHDC will 
//			be ignored by GDI+ and will not be reflected in rendering done by GDI+.
//
HDC GdiplusRenderTarget::GetHDC()
{
	UI_LOG_DEBUG(_T("%s advise: 该函数效率太低，最好不要调用"), FUNC_NAME);

	if (NULL == m_pGraphics)
		return NULL;

	POINT  p = {0,0};
	this->GetViewportOrgEx(&p);

	Gdiplus::Region r;
	m_pGraphics->GetClip(&r);
	HRGN hRgn = r.GetHRGN(m_pGraphics);  // 必须在GetHDC前面获取，否则graphics将处于busy状态

	HDC hDC = m_pGraphics->GetHDC();

	::SetViewportOrgEx(hDC, p.x, p.y, NULL);
	::SelectClipRgn(hDC, hRgn);
	::DeleteObject(hRgn);
	::SetBkMode(hDC, TRANSPARENT);

	return hDC;
}
void GdiplusRenderTarget::ReleaseHDC( HDC hDC)
{
    if (NULL == hDC)
        return;
    m_pGraphics->ReleaseHDC(hDC);
}
#endif  

// [注]：还存在一个问题，采用gdiplus绘制出的旋转图像边缘透明像素会更暗（d2d/d3d就比较纯），什么原因
void  InitGraphics(Gdiplus::Graphics& g)
{
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic); // 抗锯齿 images are scaled or rotated
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);   //  抗锯齿 lines and curves
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);  // 消除抗锯齿产生的边缘虚线

    // 注: 在拉伸一张纯图时，如果由于抗锯齿导致的边缘虚化现象，
    //     可以在绘制时加上SetWrapMode来解决
//     Gdiplus::ImageAttributes  attr;
//     attr.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
}

HDC   GdiplusRenderTarget::GetHDC()
{
    if (m_hBindDC)
        return m_hBindDC;

    if (m_pRenderBuffer)
        return m_pRenderBuffer->m_hDC;

    UIASSERT(0);
    return NULL;
}

void  GdiplusRenderTarget::ReleaseHDC(HDC hDC)
{
}

// 该HDC不需要释放 
HDC GdiplusRenderTarget::GetBindHDC()
{
	return m_hBindDC;
}

#if 0
HRGN GdiplusRenderTarget::GetClipRgn()
{
// 	Gdiplus::Region region;
// 	m_pGraphics->GetClip(&region);
// 
// 	return region.GetHRGN(m_pGraphics);

    HRGN hRgn = ::CreateRectRgn(0,0,0,0);
    if (1 != ::GetClipRgn(GetHDC(), hRgn))  // 空或者失败
    {
        ::DeleteObject(hRgn);
        hRgn = NULL;
    }
    return hRgn;
}

// 不能只设置hDC的剪裁，否则调用gdiplus绘制（如区域）时将超出
// 但gdiplus的区域设置效果咋就这么低呢
int GdiplusRenderTarget::SelectClipRgn( HRGN hRgn, int nMode)
{
    if (RGN_COPY == nMode && NULL != hRgn)
        ::SelectClipRgn(GetHDC(), hRgn);
    else
        ::ExtSelectClipRgn(GetHDC(), hRgn, nMode);  // 为gdi类型带alpha通道的位图绘制准备的

    Gdiplus::Status s = Gdiplus::Ok;
#if 0
	if (NULL == m_pGraphics)
		return 0;

	if(NULL == hRgn)
	{
		Gdiplus::Status s = m_pGraphics->ResetClip();
		return (int)s;
	}

// 	Gdiplus::Region* pRegion = Gdiplus::Region::FromHRGN(hRgn);   // 注意：在这里千万不要使用Region对象进行setclip，否则会导致当前的clip跟随vieworg而偏移
// 	if (NULL == pRegion)
// 		return ERROR;

	HRGN hRgnTemp = NULL;
	if (m_hRgnMeta)
	{
		hRgnTemp = CreateRectRgn(0,0,0,0);
		CombineRgn(hRgnTemp, hRgn, m_hRgnMeta, RGN_AND);
		hRgn = hRgnTemp;
	}
	Gdiplus::CombineMode mode = Gdiplus::CombineModeReplace;
	switch( nMode)
	{
	case RGN_AND:
		mode = Gdiplus::CombineModeIntersect;
		break;

	case RGN_COPY:
		mode = Gdiplus::CombineModeReplace;
		break;

	case RGN_DIFF:
		mode = Gdiplus::CombineModeExclude; // CombineModeComplement
		break;

	case RGN_OR:
		mode = Gdiplus::CombineModeUnion;
		break;

	case RGN_XOR:
		mode = Gdiplus::CombineModeXor;
		break;
	}

	s = m_pGraphics->SetClip(hRgn,mode);  // 效率非常低：优化！
	SAFE_DELETE_GDIOBJECT(hRgnTemp);
#endif
//	delete pRegion;
//	pRegion = NULL;

	return (int)s;
}

int  GdiplusRenderTarget::SelectClipRect( RECT* prc, uint nrcCount, int nMode)
{
	if (0 == nrcCount || NULL == prc)
	{
		return SelectClipRect(NULL, nMode);
	}
	if (1 == nrcCount)
	{
		HRGN  hRgn = CreateRectRgnIndirect(prc);
		int   nRet = SelectClipRgn(hRgn);
		DeleteObject(hRgn);
		return nRet;
	}

	RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + nrcCount*sizeof(RECT) ];
	pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType      = RDH_RECTANGLES;
	pRgnData->rdh.nCount     = nrcCount;
	pRgnData->rdh.nRgnSize   = nrcCount*sizeof(RECT);
	for (uint k = 0; k < nrcCount; k++)
	{
		RECT* prcBuffer = (RECT*)pRgnData->Buffer;
		prcBuffer[k] = prc[k];
	}
	HRGN hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + nrcCount*sizeof(RECT), pRgnData);
	SAFE_ARRAY_DELETE(pRgnData);
	int   nRet = SelectClipRgn(hRgn);
	DeleteObject(hRgn);
	return nRet;
}

BOOL GdiplusRenderTarget::GetViewportOrgEx( LPPOINT lpPoint)
{
	if (NULL == lpPoint)
	{
		return FALSE;
	}

// 	Gdiplus::Matrix  m;
// 	m_pGraphics->GetTransform(&m);
// 
// 	lpPoint->x = round(m.OffsetX());
// 	lpPoint->y = round(m.OffsetY());

    ::GetViewportOrgEx(GetHDC(), lpPoint );
	return TRUE;
}
BOOL GdiplusRenderTarget::OffsetViewportOrgEx( int xOffset, int yOffset, LPPOINT lpPoint) 
{
// 	if (lpPoint)
// 	{
// 		Gdiplus::Matrix  m;
// 		m_pGraphics->GetTransform(&m);
// 
// 		lpPoint->x = round(m.OffsetX());
// 		lpPoint->y = round(m.OffsetY());
// 	}

	::OffsetViewportOrgEx(GetHDC(), xOffset, yOffset, lpPoint);  // 为gdi类型带alpha通道的位图绘制准备的

#ifdef _DEBUG
	::GetViewportOrgEx(GetHDC(), &m_ptOffset);
#endif

// 	if (Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)xOffset,(Gdiplus::REAL)yOffset))
// 		return TRUE;
// 	else
// 		return FALSE;

    return TRUE;
}
BOOL GdiplusRenderTarget::SetViewportOrgEx( int x, int y, LPPOINT lpPoint )
{
// 	if (lpPoint)
// 	{
// 		Gdiplus::Matrix  m;
// 		m_pGraphics->GetTransform(&m);
// 		lpPoint->x = round(m.OffsetX());
// 		lpPoint->y = round(m.OffsetY());
// 	}

	::SetViewportOrgEx(GetHDC(), x, y, lpPoint);  // 为gdi类型带alpha通道的位图绘制准备的

#ifdef _DEBUG
	::GetViewportOrgEx(GetHDC(), &m_ptOffset);
#endif

// 	if (NULL == m_pGraphics)
// 		return TRUE;
// 	
// 	m_pGraphics->ResetTransform();
// 	if (Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)x,(Gdiplus::REAL)y))
// 		return TRUE;
// 	else
// 		return FALSE;

    return TRUE;
}
#endif

void  GdiplusRenderTarget::update_clip_rgn()
{
    if (m_stackClipRect.empty())
    {
        HRGN hRgn = m_arrayMetaClipRegion.CreateRgn();
        ::SelectClipRgn(GetHDC(), hRgn);
        DeleteObject(hRgn);
        return;
    }

    RECT rcIntersect = {0, 0, 1, 1};

    deque<RECT>::const_iterator iter = m_stackClipRect._Get_container().begin();
    CopyRect(&rcIntersect, &(*iter));
    iter++;
    for (; iter != m_stackClipRect._Get_container().end(); ++iter)
    {
        ::IntersectRect(&rcIntersect, &rcIntersect, &(*iter));
    }

    HRGN hRgn = NULL;
    if (!m_arrayMetaClipRegion.GetCount())
    {
        hRgn = CreateRectRgnIndirect(&rcIntersect);        
    }
    else
    {
        RectArray array = m_arrayMetaClipRegion;
        if (!array.IntersectRect(&rcIntersect))
        {
            RECT rcIntersect = {0, 0, 1, 1};
            hRgn = CreateRectRgnIndirect(&rcIntersect);           
        }
        else
        {
            hRgn = array.CreateRgn();
        }
    }

    ::SelectClipRgn(GetHDC(), hRgn);
    DeleteObject(hRgn);
}

void  GdiplusRenderTarget::SetMetaClipRegion(LPRECT prc, uint nrcCount)
{
    while (!m_stackClipRect.empty())
        m_stackClipRect.pop();

    m_arrayMetaClipRegion.CopyFromArray(prc, nrcCount);

    update_clip_rgn();
}
void  GdiplusRenderTarget::PushRelativeClipRect(LPCRECT prc)
{
    RECT rc = {0};
    if (prc)
    {
        CopyRect(&rc, prc);
        OffsetRect(&rc, m_ptOffset.x, m_ptOffset.y);
    }

    m_stackClipRect.push(rc);

    update_clip_rgn();
}
void  GdiplusRenderTarget::PopRelativeClipRect() 
{
    assert (!m_stackClipRect.empty());
    m_stackClipRect.pop();

    update_clip_rgn();
}
bool  GdiplusRenderTarget::IsRelativeRectInClip(LPCRECT prc) 
{
    assert (prc);
    if (!prc)
        return false;

    if (m_stackClipRect.empty() && m_arrayMetaClipRegion.GetCount() == 0)
        return true;

    CRect rcTest(prc);
    rcTest.OffsetRect(m_ptOffset.x, m_ptOffset.y);

    if (!m_arrayMetaClipRegion.IntersectRect(&rcTest, true))
        return false;

    if (m_stackClipRect.empty())
        return true;

    RECT rcIntersect = {0, 0, 1, 1};

    deque<RECT>::const_iterator iter = m_stackClipRect._Get_container().begin();
    CopyRect(&rcIntersect, &(*iter));
    iter++;
    for (; iter != m_stackClipRect._Get_container().end(); ++iter)
    {
        ::IntersectRect(&rcIntersect, &rcIntersect, &(*iter));
    }

    if (!IntersectRect(&rcIntersect, &rcIntersect, &rcTest))
        return false;

    return true;
}

void  GdiplusRenderTarget::SetOrigin(int x, int y) 
{
    m_ptOffset.x = x;
    m_ptOffset.y = y;

    POINT pt = {0};
    ::SetViewportOrgEx(GetHDC(), m_ptOffset.x, m_ptOffset.y, &pt);
}

void  GdiplusRenderTarget::OffsetOrigin(int x, int y) 
{
    m_ptOffset.x += x;
    m_ptOffset.y += y;

    POINT pt = {0};
    ::SetViewportOrgEx(GetHDC(), m_ptOffset.x, m_ptOffset.y, &pt);
}
void  GdiplusRenderTarget::GetOrigin(int* px, int* py) 
{
    if (px)
        *px = m_ptOffset.x;

    if (py)
        *py = m_ptOffset.y;
}


void GdiplusRenderTarget::BindHDC(HDC hDC)
{
	if (NULL == hDC)
		return;

    if (m_pRenderBuffer)
    {
        SAFE_DELETE(m_pRenderBuffer);
    }

	m_hBindDC = hDC;

// 	HBITMAP hMemBitmap = (HBITMAP) GetCurrentObject(m_hDC, OBJ_BITMAP);
// 	//	m_pGdiMemBitmap = new Gdiplus::Bitmap(hMemBitmap,NULL);  // 这种方式由于是重新创建一张位图，会大量消耗内存。
// 
// 	DIBSECTION  dibSection;
// 	GetObject(hMemBitmap, sizeof(DIBSECTION), &dibSection);
// 
// 	BYTE* pBits = (BYTE*)dibSection.dsBm.bmBits;
// 	pBits += (dibSection.dsBm.bmHeight-1)*dibSection.dsBm.bmWidthBytes;  // 将指针移到第一行数据位置
// 	m_pGdiMemBitmap = new Gdiplus::Bitmap(dibSection.dsBm.bmWidth, dibSection.dsBm.bmHeight, -dibSection.dsBm.bmWidthBytes, PixelFormat32bppARGB, (BYTE*)pBits);
}


bool  GdiplusRenderTarget::CreateRenderBuffer(IRenderTarget*  pSrcRT)
{
    if (m_pRenderBuffer)
        return false;

    m_pRenderBuffer = new RenderBuffer;
    m_pRenderBuffer->m_eGraphicsType = GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
    m_hBindDC = NULL;

    return true;
}

bool  GdiplusRenderTarget::ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight)
{
    if (!m_pRenderBuffer)
        CreateRenderBuffer(NULL);

    m_pRenderBuffer->Resize(nWidth, nHeight);
    return true;
}

void  GdiplusRenderTarget::GetRenderBufferData(ImageData*  pData)
{
	if (!m_pRenderBuffer)
		return;
	m_pRenderBuffer->GetImageData(pData);
}

bool  GdiplusRenderTarget::BeginDraw()
{
//    UIASSERT (0 == m_lDrawingRef);

//    HDC hDC = GetHDC();
	if (0 == m_lDrawingRef)
	{
        // 采用这种方式创建出来的graphics，再调用gethdc，返回的就是原始的hDC，无法使用alpha通道
		//	m_pGraphics = Gdiplus::Graphics::FromHDC(hDC);   

        // 使用内存图片创建出来的graphics，再调用gethdc，就能让HDC使用alpha通道 <-- 放弃！效率太低。直接使用HDC创建吧
		// m_pGraphics = new Gdiplus::Graphics(hDC/*m_pGdiMemBitmap*/);  

        // 启用GDIPLUS的cleartype功能
        // m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        // 消除锯齿模式
        // m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        // m_pGraphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);

		// 判断HDC是否被设置过了MetaRgn . ps. removed 20130206 在显示flash时，会频繁调用BeginDraw绘制，测试中发现
		// SelectClipRgn中的SetClip会导致效率大大的下降。因为设置meta rgn是为了加快速度的，结果却变的更慢了
// 		HRGN hRgnMeta = CreateRectRgn(0,0,0,0);
// 		if (::GetMetaRgn(hDC, hRgnMeta))
// 		{
// 			SelectClipRgn(hRgnMeta);
// 			m_hRgnMeta = hRgnMeta;
// 		}
// 		else
// 		{
// 			SAFE_DELETE_GDIOBJECT(hRgnMeta);
// 		}
	}

    m_lDrawingRef ++;
	return true;
}
void GdiplusRenderTarget::EndDraw( )
{
    -- m_lDrawingRef;
    if (0 == m_lDrawingRef)
    {
// 	    SAFE_DELETE(m_pGraphics);

	    this->SetOrigin(0,0);
	    this->SetMetaClipRegion(NULL, 0);

//	    SAFE_DELETE_GDIOBJECT(m_hRgnMeta);
    }
}

void GdiplusRenderTarget::Clear(RECT* prc)
{
// 	if (m_pGraphics)
// 	{
// 		m_pGraphics->Clear(Gdiplus::Color::MakeARGB(0,0,0,0));
// 	}
    HDC hDC = GetHDC();

    HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    if (prc)
    {
        ::FillRect(hDC, prc, hBrush);
    }
    else
    {
        BITMAP  bm;
        HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);
        if (NULL == hBitmap)
            return;

        ::GetObject(hBitmap, sizeof(bm), &bm);
        RECT  rc = {0,0, bm.bmWidth, bm.bmHeight};
        ::FillRect(hDC, &rc, hBrush);
    }
}
// BYTE* GdiplusRenderTarget::LockBits()
// {
// 	if (NULL == m_hDC)
// 		return NULL;
// 
// 	HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(m_hDC, OBJ_BITMAP);
// 	if (NULL == hBitmap)
// 		return NULL;
// 
// 	DIBSECTION dib;
// 	::GetObject(hBitmap, sizeof(DIBSECTION), &dib);
// 	return (BYTE*)dib.dsBm.bmBits;
// }
// void GdiplusRenderTarget::UnlockBits()
// {
// 	
// }
#if 0
int GdiplusRenderTarget::DrawString(const TCHAR* szText, const CRect* lpRect, UINT nFormat, IRenderFont* pRenderFont, Color col)
{
	if (NULL == lpRect || lpRect->Width() <= 0 || lpRect->Height() <= 0)
		return 0;

// 	if (NULL == m_pGraphics)
// 		return 0;

	if (NULL == pRenderFont)
	{
		UI_LOG_WARN(_T("%s pRenderFont == NULL"), FUNC_NAME);
		return -1;
	}

	if (((IRenderFont*)pRenderFont)->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
	{
		UI_LOG_WARN(_T("%s pRenderFont render type != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS"), FUNC_NAME);
		return -1;
	}

	Gdiplus::Font* pFont = ((GdiplusRenderFont*)pRenderFont)->GetFont();
	if (NULL == pFont)
	{
		UI_LOG_WARN(_T("%s NULL == pFont"), FUNC_NAME);
		return -1;
	}

	Gdiplus::RectF region(
		(Gdiplus::REAL)(lpRect->left),
		(Gdiplus::REAL)(lpRect->top), 
		(Gdiplus::REAL)(lpRect->Width()), 
		(Gdiplus::REAL)(lpRect->Height()));

	Gdiplus::Color colText;
	colText.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(col), GetGValue(col), GetBValue(col)));
	Gdiplus::SolidBrush textBrush(colText);

    // 启用GDIPLUS的cleartype功能
    // m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    // 消除锯齿模式
    // m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    // m_pGraphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);

	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
	if (nFormat & DT_CENTER)
	{
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if (nFormat & DT_RIGHT)
	{
		format.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	if (nFormat & DT_VCENTER)
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if (nFormat & DT_BOTTOM)
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentFar);
	}

    Graphics  g(GetHDC());
	// Draw string.
	Gdiplus::Status s = /*m_pGraphics->*/g.DrawString(
		szText,
		-1/*_tcslen(szText)*/,
		pFont,
		region,
		&format,
		&textBrush);

	return 0;
}
#endif
void  GdiplusRenderTarget::DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam)
{
    this->DrawStringEx(GetHDC(), pFont, pParam);
}

// 	m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
// 	m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeInvalid);
//  m_pGraphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);

// Gdiplus 文字特效 http://www.codeproject.com/Articles/42529/Outline-Text
/*static*/
void  GdiplusRenderTarget::DrawStringEx(HDC hBindDC, IRenderFont* pRenderFont, DRAWTEXTPARAM* pParam)
{
    if (NULL == pRenderFont || NULL == pParam)
        return;

    if (pParam->prc->Width() <= 0 || pParam->prc->Height() <= 0)
        return;

    if (((IRenderFont*)pRenderFont)->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
    {
        UI_LOG_WARN(_T("pRenderFont render type != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS"));
        return;
    }

    Gdiplus::Font* pFont = ((GdiplusRenderFont*)pRenderFont)->GetFont();
    if (NULL == pFont)
    {
        UI_LOG_WARN(_T("NULL == pFont"));
        return;
    }

    Gdiplus::RectF region(
        (Gdiplus::REAL)(pParam->prc->left),
        (Gdiplus::REAL)(pParam->prc->top), 
        (Gdiplus::REAL)(pParam->prc->Width()), 
        (Gdiplus::REAL)(pParam->prc->Height()));

    Gdiplus::Color colText;
    byte a = pParam->color.a;
    if (a >= 254)
        a = 254;  // 防止穿透

    colText.SetValue(Gdiplus::Color::MakeARGB(a, pParam->color.r, pParam->color.g, pParam->color.b));
    Gdiplus::SolidBrush textBrush(colText);

    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    GetStringFormatByGdiFormat(pParam->nFormatFlag, &format);

    Gdiplus::Graphics  g(hBindDC);
	InitGraphics(g);
    if (TEXT_EFFECT_NONE == pParam->nEffectFlag)
    {
        Gdiplus::TextRenderingHint eOldHint = g.GetTextRenderingHint();
        g.SetTextRenderingHint(((GdiplusRenderFont*)pRenderFont)->GetTextRenderingHint());

        // Draw string.
       g.DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            region,
            &format,
            &textBrush);

       g.SetTextRenderingHint(eOldHint);
    }
    else if (TEXT_EFFECT_HALO == pParam->nEffectFlag)
    {
        HDC hMemDC = CreateCompatibleDC(NULL);
        SetBkMode(hMemDC, TRANSPARENT);

        int nWidth = pParam->prc->Width();
        int nHeight = pParam->prc->Height();
        RECT rcMem = { 0, 0, pParam->prc->Width(), pParam->prc->Height() };
        Gdiplus::RectF regionMem(
            (Gdiplus::REAL)0,
            (Gdiplus::REAL)0, 
            (Gdiplus::REAL)nWidth, 
            (Gdiplus::REAL)nHeight);

        HBITMAP hMemBmp = CacheBitmap::GetInstance()->Create(nWidth, nHeight);
        HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp);
        //CacheBitmap::GetInstance()->Clear(0 , &rcMem);, Create会清0

        {
        Gdiplus::Graphics gMem(hMemDC);
		InitGraphics(gMem);
        Gdiplus::TextRenderingHint eOldHint = g.GetTextRenderingHint();
        gMem.SetTextRenderingHint(((GdiplusRenderFont*)pRenderFont)->GetTextRenderingHint());

        Gdiplus::Color colorTextShadow;
        byte a = pParam->bkcolor.a;
        if (a >= 254)
            a = 254;  // 防止穿透
        colorTextShadow.SetValue(Gdiplus::Color::MakeARGB(a, pParam->bkcolor.r, pParam->bkcolor.g, pParam->bkcolor.b));
        Gdiplus::SolidBrush textShadowBrush(colorTextShadow);

        // 阴影
        gMem.DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            regionMem,
            &format,
            &textShadowBrush);

        // agg模糊
//          recursive_blur<recursive_blur_calc_rgba<> > blur;
//          blur.blur(hMemBmp, (double)pParam->wParam, (LPRECT)&rcMem, 0);
        // webkit模糊
        ShadowBlur(hMemBmp, pParam->bkcolor.GetGDIValue(), &rcMem, pParam->wParam);

        // 文字
        gMem.DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            regionMem,
            &format,
            &textBrush);

        g.SetTextRenderingHint(eOldHint);
        }
    
       
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        ::AlphaBlend(hBindDC, pParam->prc->left, pParam->prc->top, nWidth, nHeight, hMemDC, 0, 0, nWidth, nHeight, bf);
        //::BitBlt(hBindDC, pParam->prc->left, pParam->prc->top, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
        ::SelectObject(hMemDC, hOldBmp);
        ::DeleteDC(hMemDC);
    }
}

void  GdiplusRenderTarget::GetStringFormatByGdiFormat(UINT nFormatFlag, Gdiplus::StringFormat* p)
{
    if (nFormatFlag & DT_CENTER)
    {
        p->SetAlignment(Gdiplus::StringAlignmentCenter);
    }
    else if (nFormatFlag & DT_RIGHT)
    {
        p->SetAlignment(Gdiplus::StringAlignmentFar);
    }
    if (nFormatFlag & DT_VCENTER)
    {
        p->SetLineAlignment(Gdiplus::StringAlignmentCenter);
    }
    else if (nFormatFlag & DT_BOTTOM)
    {
        p->SetLineAlignment(Gdiplus::StringAlignmentFar);
    }
    if (nFormatFlag & DT_SINGLELINE)
    {
        p->SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
    }
    if (nFormatFlag & DT_END_ELLIPSIS)
    {
        p->SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    }
}


IRenderPen* GdiplusRenderTarget::CreateSolidPen(int nWidth, Color* pColor)
{
    IRenderPen* p = NULL;
    GdiplusPen::CreateInstance(&p);

    if (p)
    {
        p->CreateSolidPen(nWidth, pColor);
    }
    return p;
}
IRenderPen* GdiplusRenderTarget::CreateDotPen(int nWidth, Color* pColor)
{
    IRenderPen* p = NULL;
    GdiplusPen::CreateInstance(&p);

    if (p)
    {
        p->CreateDotPen(nWidth, pColor);
    }
    return p;
}
IRenderBrush*  GdiplusRenderTarget::CreateSolidBrush(Color* pColor)
{
    UIASSERT(0);
    return NULL;
}


void GdiplusRenderTarget::FillRgn(HRGN hRgn, UI::Color* pColor)
{
	if (NULL == hRgn || NULL == pColor)
		return;

	Gdiplus::Region* pRegion = Gdiplus::Region::FromHRGN(hRgn);
	if (NULL == pRegion)
		return;

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(pColor->a, pColor->r, pColor->g, pColor->b)) ;
	Gdiplus::SolidBrush brush(color);

    Gdiplus::Graphics  g(GetHDC());
	InitGraphics(g);
	g.FillRegion(&brush, pRegion);

	delete pRegion;
	pRegion = NULL;
}


// 在给gdiplus设置了消除锯齿模式后，会导致绘制出来的rect范围不精确，有虚边效果。
// m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
// 例如一个10*10的矩形，就画出11*11的效果，并且四周有1px的虚边用于抗锯齿
//
// 2014.11.28 --> 调用g.SetPixelOffsetMode(PixelOffsetModeHighQuality);可解决该问题
//
void GdiplusRenderTarget::DrawRect(LPRECT lprc, UI::Color* pColor)
{
	if (NULL == lprc || NULL == pColor)
		return;

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(pColor->a, pColor->r, pColor->g, pColor->b));
	Gdiplus::SolidBrush brush(color);

    Gdiplus::Graphics g(GetHDC());
	InitGraphics(g);
    g.FillRectangle(&brush, lprc->left, lprc->top, lprc->right-lprc->left, lprc->bottom-lprc->top);
	//m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->right-lprc->left, lprc->bottom-lprc->top);
}

void GdiplusRenderTarget::TileRect(LPRECT lprc, IRenderBitmap* hBitmap)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	Gdiplus::TextureBrush brush(pBitmap);
    
    Gdiplus::Graphics g(GetHDC());
	InitGraphics(g);
	g.FillRectangle(&brush, 
        lprc->left, lprc->top, RECTW(lprc), RECTH(lprc));
}

// Gdiplus off-by-1 error/bug，调用DrawRectangle画出来的矩形偏大
//
// The reason that the DrawRectangle() method draws a rectangle too wide and too tall, is that the Pen is Top-Left Aligned by default. This results in the top-left edge of the pen aligning itself to the specified bounds and, therefore, produces a Rectangle whose outer edge extends outside the bounds by the width of the pen. To get the pen to draw inside the rectangle you need to decrease the size of the rectangle by the width of the pen.
// Setting Pen.Alignment to Inside should cause the Pen to draw inside the rectangle, but for a 1 pixel width pen, this setting has no affect. This is the real Bug.
void  FixDrawRectangle(Gdiplus::Graphics& g, LPCRECT prc, Gdiplus::Color& color, int nPenWidth=1)
{
	// Gdiplus::SmoothingModeAntiAlias仍然会将像素扩大1px。因此直接使用SmoothingModeDefault
	Gdiplus::SmoothingMode eOldSmoothMode = g.GetSmoothingMode();
    Gdiplus::PixelOffsetMode eOldPixelOffsetMode = g.GetPixelOffsetMode();

    g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);  

	Gdiplus::Pen pen(color, (Gdiplus::REAL)nPenWidth);

	// 因为pen默认的绘制方式是align center，因此对于一个pen width=2，大小为10*10的矩形
	// 为其描边时，里外各画一像素，因此就形成了一个 (-1, -1, 11, 11) 的矩形。
	// 对于这种情况，将pen的对齐方式修改为内对齐即可。
	if (nPenWidth > 1)
	{
		pen.SetAlignment(Gdiplus::PenAlignmentInset);
		g.DrawRectangle(&pen,   
				prc->left,
				prc->top, 
				prc->right-prc->left, 
				prc->bottom-prc->top  
			);  
	}
	// penwidth==1时，使用insert对齐不管用。原因未知。
	else
	{
		g.DrawRectangle(&pen,   
			prc->left,
			prc->top, 
			prc->right-prc->left-1, 
			prc->bottom-prc->top-1  
			);  
	}

	g.SetSmoothingMode(eOldSmoothMode);
    g.SetPixelOffsetMode(eOldPixelOffsetMode);  
}

void GdiplusRenderTarget::Rectangle(LPRECT lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack)
{
    CRect rc(lprc);
	if (false == bNullBack)
	{
		this->DrawRect(&rc, pColBack);
	}

    if (pColBorder)
    {
	    Gdiplus::Color color;
	    color.SetValue(
			Gdiplus::Color::MakeARGB(
				pColBorder->a,
				pColBorder->r, 
				pColBorder->g, 
				pColBorder->b));
        Gdiplus::Graphics g(GetHDC());
		FixDrawRectangle(g, &rc, color, 1);
    }
}

void GdiplusRenderTarget::DrawFocusRect(LPRECT lprc)
{
    Gdiplus::Graphics g(GetHDC());
	InitGraphics(g);

	Gdiplus::Pen p( Gdiplus::Color(254,0,0,0));
	p.SetDashStyle( Gdiplus::DashStyleDot);
	g.DrawRectangle(&p,   
		lprc->left, lprc->top, 
		RECTW(lprc), RECTH(lprc) 
		);  
}

void GdiplusRenderTarget::DrawLine(int x1, int y1, int x2, int y2, IRenderPen* pPen)
{
	if (NULL == pPen)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusPen* pGdiplusPen = (GdiplusPen*)pPen;

	Gdiplus::Point p1(x1, y1);
	Gdiplus::Point p2(x2, y2);
    Gdiplus::Graphics g(GetHDC());
	InitGraphics(g);

	g.DrawLine(pGdiplusPen->m_pPen, p1, p2);
}
void GdiplusRenderTarget::DrawPolyline(POINT* lppt, int nCount, IRenderPen* pPen)
{
	if (NULL == pPen || NULL == lppt)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusPen* pGdiplusPen = (GdiplusPen*)pPen;
	Gdiplus::Point *p = new Gdiplus::Point[nCount];
	for (int i = 0; i < nCount; i++)
	{
		p[i].X = lppt[i].x;
		p[i].Y = lppt[i].y;
	}
    Gdiplus::Graphics g(GetHDC());
	InitGraphics(g);

	g.DrawLines(pGdiplusPen->m_pPen, p, nCount);
	SAFE_ARRAY_DELETE(p);
}

void GdiplusRenderTarget::GradientFillH(LPRECT lprc, COLORREF colFrom, COLORREF colTo)
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect(lprc->left, lprc->top, RECTW(lprc), RECTH(lprc));
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeHorizontal);

    Gdiplus::Graphics g(GetHDC());
	InitGraphics(g);
	g.FillRectangle(&brush, lprc->left, lprc->top, RECTW(lprc), RECTH(lprc));
}

void GdiplusRenderTarget::GradientFillV(LPRECT lprc, COLORREF colFrom, COLORREF colTo)
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect( lprc->left, lprc->top, RECTW(lprc), RECTH(lprc));
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeVertical);

    Gdiplus::Graphics g(GetHDC());
	InitGraphics(g);
	g.FillRectangle(&brush, lprc->left, lprc->top, RECTW(lprc), RECTH(lprc));
}

void GdiplusRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop)
{
	UIASSERT(0);
	// TODO: 该函数还会被调用吗

// 	if (NULL == pSrcHDC)
// 		return;
// 	if (pSrcHDC->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
// 		return;
// 
// 	GdiplusMemRenderDC* pMemRDC = (GdiplusMemRenderDC*)pSrcHDC;
// 	m_pGraphics->DrawImage(pMemRDC->GetMemBitmap(), xDest,yDest,xSrc,ySrc, wDest,hDest, Gdiplus::UnitPixel);
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int x, int y)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	pGraphics->DrawImage( pBitmap,x, y, pBitmap->GetWidth(), pBitmap->GetHeight());
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
{
	if (NULL == pBitmap)
		return;

	if (pBitmap->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = static_cast<GdiplusRenderBitmap*>(pBitmap);
	
	Gdiplus::Bitmap* p = pRenderBitmap->GetBitmap();
	if (NULL == p)
		return;

	pGraphics->DrawImage( p, xDest, yDest, xSrc, ySrc, pBitmap->GetWidth(), pBitmap->GetHeight(), Gdiplus::UnitPixel);
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, Gdiplus::ImageAttributes* pAttr)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	Gdiplus::RectF rcDst( (Gdiplus::REAL)xDest, (Gdiplus::REAL)yDest, (Gdiplus::REAL)nDestWidth, (Gdiplus::REAL)nDestHeight);
	pGraphics->DrawImage( pBitmap, rcDst, (Gdiplus::REAL)xSrc, (Gdiplus::REAL)ySrc, (Gdiplus::REAL)nSrcWidth, (Gdiplus::REAL)nSrcHeight, Gdiplus::UnitPixel, pAttr);
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
					int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
					C9Region* pImage9Region, bool bDrawCenter, Gdiplus::ImageAttributes* pAttr)
{
	if (NULL == hBitmap)
		return;

	if (NULL == pImage9Region)
		return GdiplusRenderTarget::DrawBitmap(pGraphics, hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, pAttr);

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;


	// 1. topleft
	Gdiplus::RectF rcDst1( (Gdiplus::REAL)xDest, (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)pImage9Region->topleft, (Gdiplus::REAL)pImage9Region->top);
	pGraphics->DrawImage( pBitmap, rcDst1, 

		(Gdiplus::REAL)xSrc, 
		(Gdiplus::REAL)ySrc,
		(Gdiplus::REAL)pImage9Region->topleft,
		(Gdiplus::REAL)pImage9Region->top,

		Gdiplus::UnitPixel, pAttr
		);

	// 2. top
	Gdiplus::RectF rcDst2( (Gdiplus::REAL)(xDest + pImage9Region->topleft), (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)(nDestWidth - pImage9Region->topleft - pImage9Region->topright), (Gdiplus::REAL)pImage9Region->top);
	pGraphics->DrawImage( pBitmap, rcDst2,

		(Gdiplus::REAL)(xSrc + pImage9Region->topleft),                                         // xSrc
		(Gdiplus::REAL)ySrc,                                                               // ySrc
		(Gdiplus::REAL)(nSrcWidth - pImage9Region->topleft - pImage9Region->topright),       // nSrcWidth
		(Gdiplus::REAL)pImage9Region->top ,

		Gdiplus::UnitPixel, pAttr
		);                                                 

	// 3. topright
	Gdiplus::RectF rcDst3( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->topright), (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)(pImage9Region->topright), (Gdiplus::REAL)pImage9Region->top);
	pGraphics->DrawImage( pBitmap, rcDst3,

		(Gdiplus::REAL)(xSrc + nSrcWidth - pImage9Region->topright),
		(Gdiplus::REAL)ySrc, 
		(Gdiplus::REAL)pImage9Region->topright,
		(Gdiplus::REAL)pImage9Region->top,

		Gdiplus::UnitPixel, pAttr
		);

	// 4. left
	Gdiplus::RectF rcDst4( (Gdiplus::REAL)(xDest), (Gdiplus::REAL)(yDest + pImage9Region->top), 
							(Gdiplus::REAL)(pImage9Region->left), (Gdiplus::REAL)(nDestHeight - pImage9Region->top- pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst4,

		(Gdiplus::REAL)xSrc ,                                                        // xSrc
		(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
		(Gdiplus::REAL)pImage9Region->left,                                         // nSrcWidth
		(Gdiplus::REAL)(nSrcHeight - pImage9Region->top - pImage9Region->bottom),      // nSrcHeight

		Gdiplus::UnitPixel, pAttr
		);

	// 5. center

	if (bDrawCenter)
	{
		Gdiplus::RectF rcDst5( (Gdiplus::REAL)(xDest + pImage9Region->left), (Gdiplus::REAL)(yDest + pImage9Region->top), 
								(Gdiplus::REAL)(nDestWidth  - pImage9Region->left - pImage9Region->right), (Gdiplus::REAL)(nDestHeight - pImage9Region->top  - pImage9Region->bottom));
		pGraphics->DrawImage( pBitmap, rcDst5,

			(Gdiplus::REAL)(xSrc + pImage9Region->left),                                         // xSrc
			(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
			(Gdiplus::REAL)(nSrcWidth  - pImage9Region->left - pImage9Region->right),           // nSrcWidth
			(Gdiplus::REAL)(nSrcHeight - pImage9Region->top  - pImage9Region->bottom),       // nSrcHeight

			Gdiplus::UnitPixel, pAttr
			); 
	}

	// 6. right
	Gdiplus::RectF rcDst6( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->right), (Gdiplus::REAL)(yDest + pImage9Region->top), 
							(Gdiplus::REAL)(pImage9Region->right), (Gdiplus::REAL)(nDestHeight - pImage9Region->top - pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst6,

		(Gdiplus::REAL)(xSrc + nSrcWidth - pImage9Region->right),                            // xSrc
		(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
		(Gdiplus::REAL)(pImage9Region->right),                                        // nSrcWidth
		(Gdiplus::REAL)(nSrcHeight - pImage9Region->top - pImage9Region->bottom),        // nSrcHeight

		Gdiplus::UnitPixel, pAttr
		);

	// 7. bottomleft
	Gdiplus::RectF rcDst7( (Gdiplus::REAL)(xDest), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(pImage9Region->bottomleft), (Gdiplus::REAL)(pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst7,

		(Gdiplus::REAL)xSrc, 
		(Gdiplus::REAL)(ySrc + nSrcHeight - pImage9Region->bottom),
		(Gdiplus::REAL)pImage9Region->bottomleft, 
		(Gdiplus::REAL)pImage9Region->bottom,

		Gdiplus::UnitPixel, pAttr
		);

	// 8. bottom
	Gdiplus::RectF rcDst8( (Gdiplus::REAL)(xDest + pImage9Region->bottomleft), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(nDestWidth - pImage9Region->bottomleft - pImage9Region->bottomright), (Gdiplus::REAL)(pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst8,

		(Gdiplus::REAL)xSrc + pImage9Region->bottomleft,                                   // xSrc
		(Gdiplus::REAL)ySrc + nSrcHeight - pImage9Region->bottom,                          // ySrc
		(Gdiplus::REAL)nSrcWidth - pImage9Region->bottomleft - pImage9Region->bottomright, // nSrcWidth
		(Gdiplus::REAL)pImage9Region->bottom,                                         // nSrcHeight

		Gdiplus::UnitPixel, pAttr
		);

	// 9. bottomright
	Gdiplus::RectF rcDst9( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->bottomright), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(pImage9Region->bottomright), (Gdiplus::REAL)(pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst9,

		(Gdiplus::REAL)(xSrc + nSrcWidth  - pImage9Region->bottomright),
		(Gdiplus::REAL)(ySrc + nSrcHeight - pImage9Region->bottom), 
		(Gdiplus::REAL)pImage9Region->bottomright,
		(Gdiplus::REAL)pImage9Region->bottom,

		Gdiplus::UnitPixel, pAttr
		);
}
void GdiplusRenderTarget::ImageList_Draw( IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	int xSrc = col * cx;
	int ySrc = row * cy;

// 	if (xSrc > pBitmap->GetWidth())
// 		return false;
// 	if (ySrc > pBitmap->GetHeight())
// 		return false;

	//	m_pGraphics->DrawImage(pBitmap, x,y,xSrc,ySrc, cx,cy, Gdiplus::UnitPixel);  <-- 使用这个版本会导致图片被放大...原因未知
	Gdiplus::RectF rcDst( (Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
    Gdiplus::Graphics  g(GetHDC());
	InitGraphics(g);
	g.DrawImage( pBitmap, rcDst, (Gdiplus::REAL)xSrc, (Gdiplus::REAL)ySrc, (Gdiplus::REAL)cx, (Gdiplus::REAL)cy, Gdiplus::UnitPixel);
}


void GdiplusRenderTarget::DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == hBitmap || NULL == pParam)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;

	if (p->GetGraphicsRenderLibraryType() == GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
	{
		GdiRenderTarget::DrawBitmapEx(GetHDC(), hBitmap, pParam);
		return;
	}

	GdiplusRenderTarget::DrawBitmapEx(GetHDC(), p, pParam);
}

// [注]:这里的alpha绘制效率很低，特别是在实现动画渐变效果时，效果很不好，最好还是用gdi的alpha blend
void GdiplusRenderTarget::DrawBitmapEx(HDC hBindDC, IRenderBitmap* p, DRAWBITMAPPARAM* pParam)
{
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	Gdiplus::Bitmap* pBitmap = ((GdiplusRenderBitmap*)p)->GetBitmap();
	if (NULL == pBitmap)
		return;

    Gdiplus::Graphics  g(hBindDC);
	InitGraphics(g);

	// 利用颜色矩阵来直接绘制灰度图
	Gdiplus::ImageAttributes* pImageAttribute = NULL;
	Gdiplus::ImageAttributes  imageAttribute;

	float fAlphaBlend = 1.0f;
	if (pParam->nAlpha != 255)
		fAlphaBlend = pParam->nAlpha / 255.0f;

	if (pParam->nFlag & DRAW_BITMAP_DISABLE)
	{
		const Gdiplus::REAL r = (Gdiplus::REAL)0.3;
		const Gdiplus::REAL g = (Gdiplus::REAL)0.59;
		const Gdiplus::REAL b = (Gdiplus::REAL)0.11;
		const Gdiplus::ColorMatrix matrix =
				{r,    r,   r,     0.0f,  0.0f,      // Red
				 g,    g,    g,    0.0f,  0.0f,      // Green
				 b,    b,    b,    0.0f,  0.0f,      // Blue
				 0.0f, 0.0f, 0.0f, fAlphaBlend,  0.0f,      // Alpha
				 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};     
		imageAttribute.SetColorMatrix(&matrix);
		pImageAttribute = &imageAttribute;
	}
	else if (pParam->nAlpha != 255)
	{
		const Gdiplus::ColorMatrix matrix =
				{1.0f, 0.0f, 0.0f,  0.0f,  0.0f,      // Red
				 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,      // Green
				 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,      // Blue
				 0.0f, 0.0f, 0.0f, fAlphaBlend,  0.0f,      // Alpha
				 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};     
		imageAttribute.SetColorMatrix(&matrix);
		pImageAttribute = &imageAttribute;
	}

	if (pParam->nFlag & DRAW_BITMAP_BITBLT)
	{
		Gdiplus::REAL nW = (Gdiplus::REAL)min(pParam->wSrc,pParam->wDest);
		Gdiplus::REAL nH = (Gdiplus::REAL)min(pParam->hSrc,pParam->hDest);
		Gdiplus::RectF destRect((Gdiplus::REAL)pParam->xDest, (Gdiplus::REAL)pParam->yDest, nW, nH);
		g.DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, nW, nH, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+(int)nW, pParam->yDest+(int)nH);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH)
	{
		GdiplusRenderTarget::DrawBitmap(&g, p, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest,
			pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pImageAttribute);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH_BORDER)
	{
		GdiplusRenderTarget::DrawBitmap(&g, p, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest,
			pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, false, pImageAttribute);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_TILE)
	{
		Gdiplus::TextureBrush brush(pBitmap);
		g.FillRectangle(&brush, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_CENTER)
	{
		int x = pParam->xDest + (pParam->wDest - pParam->wSrc)/2;
		int y = pParam->yDest + (pParam->hDest - pParam->hSrc)/2;

		Gdiplus::RectF destRect((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc);
		g.DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(x, y, x+pParam->wSrc, y+pParam->hSrc);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_ADAPT)
	{
		if (pParam->wSrc == 0 || pParam->hSrc == 0)
			return;

		if (pParam->wDest == 0 || pParam->hDest == 0)
			return;

		bool bNeedToStretch = false;
		int  wImage = pParam->wSrc;
		int  hImage = pParam->hSrc;

		if (pParam->wDest < pParam->wSrc || pParam->hDest < pParam->hSrc)
		{
			bNeedToStretch = true;

			double tan_x_y_image = (double)pParam->wSrc / (double)pParam->hSrc;
			double tan_x_y_dest = (double)pParam->wDest / (double)pParam->hDest;

			if (tan_x_y_image > tan_x_y_dest) // 横向占满
			{
				wImage = pParam->wDest;
				hImage = (int)((double)wImage/tan_x_y_image);
			}
			else   // 纵向占满
			{
				hImage = pParam->hDest;
				wImage = (int)(hImage*tan_x_y_image);
			}
		}

		// 计算图片显示位置
		int xDisplayPos = pParam->xDest + (pParam->wDest-wImage)/2;
		int yDisplayPos = pParam->yDest + (pParam->hDest-hImage)/2;

		if (bNeedToStretch)
		{
			GdiplusRenderTarget::DrawBitmap(&g, p, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pImageAttribute);
		}
		else
		{
			Gdiplus::RectF destRect((Gdiplus::REAL)xDisplayPos, (Gdiplus::REAL)yDisplayPos, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc);
			g.DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);
		}

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDisplayPos, yDisplayPos, xDisplayPos+wImage, yDisplayPos+hImage);
        }
	}
    else if (pParam->nFlag & DRAW_BITMAP_BITBLT_RIGHTTOP)
    {
        Gdiplus::REAL nW = (Gdiplus::REAL)min(pParam->wSrc,pParam->wDest);
        Gdiplus::REAL nH = (Gdiplus::REAL)min(pParam->hSrc,pParam->hDest);

        int xDest = pParam->xDest + (pParam->wDest-(int)nW);
        // int xSrc = pParam->xSrc+ (pParam->wSrc-(int)nW);

        Gdiplus::RectF destRect((Gdiplus::REAL)xDest, (Gdiplus::REAL)pParam->yDest, nW, nH);
        g.DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, nW, nH, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDest, pParam->yDest, xDest+(int)nW, pParam->yDest+(int)nH);
        }
    }
    else if (pParam->nFlag & DRAW_BITMAP_STRETCH_ROUNDCORNER)
    {
        HDC hDC = Image::GetCacheDC();
        HBITMAP hCacheBitmap = CacheBitmap::GetInstance()->Create(pParam->wDest, pParam->hDest);
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, hCacheBitmap);

        // 绘制到临时位置
        {
            Gdiplus::Graphics g(hDC);
			InitGraphics(g);

            Gdiplus::ImageAttributes attr;
            if (!pImageAttribute)
                pImageAttribute = &attr;
            pImageAttribute->SetWrapMode(Gdiplus::WrapModeTileFlipXY);   // 不加这句边缘绘制出来有问题

            GdiplusRenderTarget::DrawBitmap(&g, p, 0, 0, pParam->wDest, pParam->hDest,
                pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pImageAttribute);
        }

        // 圆角处理
        int& w = pParam->wDest;
        int& h = pParam->hDest;
        if (w > 8 && h > 8)
        {
            BITMAP bm = {0};
            ::GetObject(hCacheBitmap, sizeof(bm), &bm);
            byte* pBits = (byte*)bm.bmBits;
            int   nPitch = bm.bmWidthBytes;

            // 25%透
            POINT ptAlpha0[4] = { {0, 0}, {w-1, 0}, {0, h-1}, {w-1, h-1} };
            for (int i = 0; i < 4; i++)
            {
                byte* pAlpha = pBits + (ptAlpha0[i].y*nPitch + (ptAlpha0[i].x<<2));
                byte a = ((int)*(pAlpha+3)) >> 4;
                *pAlpha = (*pAlpha) * a >> 8;   // r
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;   // g
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;   // b
                pAlpha++;
                *pAlpha = a;                    // a
            }

            // 50%透
            POINT ptAlpha1[8] = { {1, 0}, {w-2, 0}, {0, 1}, {w-1,1},  {0, h-2}, {w-1, h-2}, {1, h-1}, {w-2, h-1} };
            for (int i = 0; i < 8; i++)
            {
                byte* pAlpha = pBits + (ptAlpha1[i].y*nPitch + (ptAlpha1[i].x<<2));
                byte a = ((int)*(pAlpha+3)) >> 2;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = a;
            }

            // 75%
            POINT ptAlpha2[8] = { {2, 0}, {w-3, 0}, {0, 2}, {w-1,2},  {0, h-3}, {w-1, h-3}, {2, h-1}, {w-3, h-1} };
            for (int i = 0; i < 8; i++)
            {
                byte* pAlpha = pBits + (ptAlpha2[i].y*nPitch + (ptAlpha2[i].x<<2));
                byte a = ((int)*(pAlpha+3)) * 3 >> 2;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = a;
            }
        }

        // 提交
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, pParam->nAlpha, AC_SRC_ALPHA};
        AlphaBlend(hBindDC, pParam->xDest, pParam->yDest, w, h, hDC, 0, 0, w, h, bf);

        ::SelectObject(hDC, hOldBitmap);
        Image::ReleaseCacheDC(hDC);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
    }
}

#pragma endregion

void  GdiplusRenderTarget::Save(const TCHAR*  szPath)
{
    if (m_pRenderBuffer)
    {
        m_pRenderBuffer->Dump();
    }
}

void  GdiplusRenderTarget::Render2DC(HDC hDstDC, Render2TargetParam* pParam)
{
    int& xDst = pParam->xDst;
    int& yDst = pParam->yDst; 
    int& wDst = pParam->wDst; 
    int& hDst = pParam->hDst; 
    int& xSrc = pParam->xSrc; 
    int& ySrc = pParam->ySrc; 
    int& wSrc = pParam->wSrc; 
    int& hSrc = pParam->hSrc; 
    bool& bAlphaBlend = pParam->bAlphaBlend;
    byte& opacity = pParam->opacity;

    HDC hDC = GetHDC();
    UIASSERT (hDC != hDstDC);

	int nOldGraphicsMode = 0;
	if (pParam->pTransform2d)
	{
		nOldGraphicsMode = ::GetGraphicsMode(hDstDC);
		::SetGraphicsMode(hDstDC, GM_ADVANCED);

		XFORM xForm = {
			pParam->pTransform2d->m11, pParam->pTransform2d->m12,
			pParam->pTransform2d->m21, pParam->pTransform2d->m22,
			pParam->pTransform2d->m31, pParam->pTransform2d->m32,
		};
		::SetWorldTransform(hDstDC, &xForm);
	}

    if (bAlphaBlend)
    {
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA};
        ::AlphaBlend(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, wSrc, hSrc, bf);
    }
    else
    {
        if (wDst == wSrc && hDst == hSrc)
        {
            ::BitBlt(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, SRCCOPY);
        }
        else
        {
            ::StretchBlt(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
        }
    }
	if (pParam->pTransform2d)
	{
		ModifyWorldTransform(hDstDC, NULL, MWT_IDENTITY);
		::SetGraphicsMode(hDstDC, nOldGraphicsMode);
	}
}

void  GdiplusRenderTarget::Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam)
{
    if (!pDst)
        return;

    if (pDst->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
    {
        UI_LOG_WARN(_T("Graphcis Render library is wrong"));
        return;
    }

	GdiplusRenderTarget* pGdipDst = static_cast<GdiplusRenderTarget*>(pDst);
	if (!m_pRenderBuffer)
	{
		return Render2DC(pGdipDst->GetHDC(), pParam);
	}

	if (pParam->pTransform2d)
	{
		ImageData  imageData;
		m_pRenderBuffer->GetImageData(&imageData);
		
		Gdiplus::Matrix  matrix;
		matrix.SetElements(
			pParam->pTransform2d->m11, pParam->pTransform2d->m12,
			pParam->pTransform2d->m21, pParam->pTransform2d->m22,
			pParam->pTransform2d->m31, pParam->pTransform2d->m32);

		Gdiplus::Graphics  gDst(pGdipDst->GetHDC());
		InitGraphics(gDst);
		gDst.MultiplyTransform(&matrix); // 与当前的偏移矩阵相乘

		// 由于Gdi的变换不带抗锯齿，因此使用gdiplus的方式来实现 
		Gdiplus::Bitmap*  pBitmap = new Gdiplus::Bitmap(
				imageData.m_nWidth, 
				imageData.m_nHeight,
				abs(imageData.m_nStride),
				PixelFormat32bppARGB,
				(BYTE*)imageData.m_pScan0);

		Gdiplus::RectF rcDst1( (Gdiplus::REAL)pParam->xDst, (Gdiplus::REAL)pParam->yDst, 
			(Gdiplus::REAL)pParam->wDst, (Gdiplus::REAL)pParam->hDst);
		gDst.DrawImage(pBitmap, rcDst1, 
			(Gdiplus::REAL)pParam->xSrc, 
			(Gdiplus::REAL)pParam->ySrc,
			(Gdiplus::REAL)pParam->wSrc,
			(Gdiplus::REAL)pParam->hSrc,
			Gdiplus::UnitPixel, NULL
			);

	//     POINT  ptOffset;
	//     ::GetViewportOrgEx(pGdipDst->GetHDC(), &ptOffset);
	// 	pGdipDst->m_pGraphics->ResetTransform();
	// 	pGdipDst->m_pGraphics->TranslateTransform((Gdiplus::REAL)ptOffset.x, (Gdiplus::REAL)ptOffset.y);

		SAFE_DELETE(pBitmap);
	}
	else if (pParam->pTransform3d)
	{
		QuadPerspectiveTextureMapping textmapping;

		Matrix44  matrix(pParam->pTransform3d);
		RECT rcSrc = {pParam->xSrc, pParam->ySrc, pParam->xSrc+pParam->wSrc, pParam->ySrc+pParam->hSrc};

		// 设置纹理映射坐标
		QUAD quad;
		matrix.MapRect2Quad(&rcSrc, &quad);  // 变换
		textmapping.SetQuad(&quad, &rcSrc);

		// 设置纹理坐标
		textmapping.SetTextureRect(&rcSrc);

		// 设置纹理(src)和目标缓存(dest)
		ImageData  srcBuffer;
		this->GetRenderBufferData(&srcBuffer);
		textmapping.SetSrcBuffer(&srcBuffer);

		ImageData  dstBuffer;
		pGdipDst->GetRenderBufferData(&dstBuffer);
		textmapping.SetDstBuffer(&dstBuffer);

		// 渲染位置和剪裁区域。 参见Object::DrawChildObject的剪裁处理方式
		POINT  ptOffsetDst;
		pGdipDst->GetOrigin((int*)&ptOffsetDst.x, (int*)&ptOffsetDst.y);
		POINT ptDraw = {ptOffsetDst.x + pParam->xDst, ptOffsetDst.y + pParam->yDst};

		RECT  rcClip = {0};
		HDC   hDstDC = pGdipDst->GetHDC();
		HRGN  hCurRgn = CreateRectRgnIndirect(&rcClip);
		::GetClipRgn(pGdipDst->GetHDC(), hCurRgn);
		::GetRgnBox(hCurRgn, &rcClip);
		DeleteObject(hCurRgn);
		hCurRgn = NULL;

        ::GetViewportOrgEx(hDstDC, &ptOffsetDst);
		textmapping.Do(&ptDraw, &rcClip, pParam->opacity);
		return;
	}
	else
	{
		return Render2DC(pGdipDst->GetHDC(), pParam);
	}
}
void  GdiplusRenderTarget::Upload2Gpu(IGpuRenderLayer* p, LPRECT prcArray, int nCount)
{
    if (m_pRenderBuffer)
    {
        m_pRenderBuffer->Upload2Gpu(p, prcArray, nCount);
    }
}