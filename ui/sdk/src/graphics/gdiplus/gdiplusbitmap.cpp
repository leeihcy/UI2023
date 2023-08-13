#include "stdafx.h"
#include "gdiplusbitmap.h"

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);


//
//	Remark
//
//		Q: 为什么用Gdiplus在透明窗口上面绘制文字不能用alpha 255?
//		A: (从网上抄的，不一定正确)  alpha值会由于Red值的进位导致255变成0
//


void GdiplusRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return ;

	GdiplusRenderBitmap* p = new GdiplusRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}

GdiplusImageListRenderBitmap::GdiplusImageListRenderBitmap()
{
	m_nCount = 0;
	m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
}
GdiplusImageListRenderBitmap::~GdiplusImageListRenderBitmap()
{

}
void GdiplusImageListRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GdiplusImageListRenderBitmap* p = new GdiplusImageListRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}

int GdiplusImageListRenderBitmap::GetItemWidth()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_V:
		return GetWidth();

	case IMAGELIST_LAYOUT_TYPE_H:
		return GetWidth()/m_nCount;
	}

	return 0;
}
int GdiplusImageListRenderBitmap::GetItemHeight()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_H:
		return GetHeight();

	case IMAGELIST_LAYOUT_TYPE_V:
		return GetHeight()/m_nCount;
	}

	return 0;
}
int GdiplusImageListRenderBitmap::GetItemCount()
{
	return m_nCount;
}
IMAGELIST_LAYOUT_TYPE GdiplusImageListRenderBitmap::GetLayoutType()
{
	return m_eLayout;
}
void  GdiplusImageListRenderBitmap::SetItemCount(int n)
{
    m_nCount = n;
}
void  GdiplusImageListRenderBitmap::SetLayoutType(IMAGELIST_LAYOUT_TYPE e)
{
    m_eLayout = e;
}
bool GdiplusImageListRenderBitmap::GetIndexPos(int nIndex, POINT* pPoint)
{
	if (NULL == pPoint)
		return false;

	pPoint->x = pPoint->y = 0;
	if (nIndex > m_nCount)
		return false;

	if (IMAGELIST_LAYOUT_TYPE_H == m_eLayout)
	{
		pPoint->x = nIndex*GetItemWidth();
		pPoint->y = 0;
	}
	else if (IMAGELIST_LAYOUT_TYPE_V == m_eLayout)
	{
		pPoint->x = 0;
		pPoint->y = nIndex*GetItemHeight();
	}
	else 
		return false;

	return true;
}
GdiplusIconRenderBitmap::GdiplusIconRenderBitmap()
{
	m_nIconWidth = m_nIconHeight = 16;
	m_hBitmapToFixIcon = NULL;
}
GdiplusIconRenderBitmap::~GdiplusIconRenderBitmap()
{
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);
}
void GdiplusIconRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GdiplusIconRenderBitmap* p = new GdiplusIconRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}

// 强制加载为支持32位alpha的
bool GdiplusIconRenderBitmap::LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e)
{
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);

	HICON hIcon = (HICON)::LoadImage (NULL, szPath, IMAGE_ICON,m_nIconWidth,m_nIconHeight, LR_LOADFROMFILE);
	if (NULL == hIcon)
		return false;
#if 1

#else
	HDC hMemDC = UI_GetCacheDC();

	Image image;
	image.Create(m_nIconWidth,m_nIconHeight, 32, Image::createAlphaChannel);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, (HBITMAP)image);

	::DrawIconEx(hMemDC, 0,0, hIcon, m_nIconWidth,m_nIconHeight, 0, NULL, DI_NORMAL);
	::SelectObject(hMemDC, hOldBmp);
	::UI_ReleaseCacheDC(hMemDC);
	::DestroyIcon(hIcon);

	m_pBitmap = new Gdiplus::Bitmap(
		m_nIconWidth,
		m_nIconHeight,
		image.GetPitch(),
		PixelFormat32bppARGB,
		(BYTE*)image.GetBits() );
	m_hBitmapToFixIcon = image.Detach();    // Bitmap不负责保存bits数据，因此image.m_hBitmap不能提前释放，需要增加一个成员变量保存该句柄
#endif
	return true;
}

SIZE  GdiplusIconRenderBitmap::GetDrawSize()
{
    SIZE s = { m_nIconWidth, m_nIconHeight };
    return s;
}
void  GdiplusIconRenderBitmap::SetDrawSize(SIZE* ps)
{
    if (!ps)
        return;

    ps->cx = m_nIconWidth;
    ps->cy = m_nIconHeight;
}