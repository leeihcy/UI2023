#include "include/inc.h"
#include "skia_render.h"

namespace ui {

SkiaRenderTarget::SkiaRenderTarget(bool bNeedAlphaChannel) {
  m_bNeedAlphaChannel = bNeedAlphaChannel;
}


SkiaRenderTarget::~SkiaRenderTarget()
{
    // SAFE_DELETE(m_pRenderBuffer);
 	// m_hBindDC = nullptr;
}

void  SkiaRenderTarget::Release()
{
    delete this;
}

// 该HDC不需要释放 
// HDC SkiaRenderTarget::GetBindHDC()
// {
// 	return m_hBindDC;
// }


void  SkiaRenderTarget::SetMetaClipRegion(RECT* prc, uint nrcCount)
{
   UIASSERT(false);
}

void  SkiaRenderTarget::PushRelativeClipRect(const RECT* prc)
{
  UIASSERT(false);
}

void  SkiaRenderTarget::PopRelativeClipRect() 
{
 UIASSERT(false);
}

bool  SkiaRenderTarget::IsRelativeRectInClip(const RECT* prc) 
{
  UIASSERT(false);
  return false;
}

void  SkiaRenderTarget::SetOrigin(int x, int y) 
{
	// if (m_ptOffset.x == x && m_ptOffs-
}

void  SkiaRenderTarget::OffsetOrigin(int x, int y) 
{
    // m_ptOffset.x += x;
    // m_ptOffset.y += y;

    // POINT pt = {0};
    // ::SetViewportOrgEx(GetHDC(), m_ptOffset.x, m_ptOffset.y, &pt);
}
void  SkiaRenderTarget::GetOrigin(int* px, int* py) 
{
    // if (px)
    //     *px = m_ptOffset.x;

    // if (py)
    //     *py = m_ptOffset.y;
}

bool  SkiaRenderTarget::CreateRenderBuffer(IRenderTarget*  pSrcRT)
{
    // if (m_pRenderBuffer)
    //     return false;

    // m_pRenderBuffer = new RenderBuffer;
    // m_pRenderBuffer->m_eGraphicsType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
    // m_hBindDC = nullptr;

    // return true;
    UIASSERT(false);
  return false;
}

bool  SkiaRenderTarget::ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight)
{
    // if (!m_pRenderBuffer)
    //     CreateRenderBuffer(nullptr);

    // m_pRenderBuffer->Resize(nWidth, nHeight);
    // return true;
    UIASSERT(false);
  return false;
}

void  SkiaRenderTarget::GetRenderBufferData(ImageData*  pData)
{
	// if (!m_pRenderBuffer)
	// 	return;
	// m_pRenderBuffer->GetImageData(pData);
}


bool SkiaRenderTarget::BeginDraw()
{
  UIASSERT(false);
}

void SkiaRenderTarget::EndDraw()
{
   UIASSERT(false);
}

void SkiaRenderTarget::Clear(RECT* prc)
{
    UIASSERT(false);
}

void  SkiaRenderTarget::DrawString(IRenderFont* pRenderFont, DRAWTEXTPARAM* pParam)
{
  UIASSERT(false);
}


void SkiaRenderTarget::FillRgn( HRGN hRgn, ui::Color* pColor)
{
UIASSERT(false);
}

void SkiaRenderTarget::DrawRect(RECT* lprc, ui::Color* pColor)
{
  UIASSERT(false);
}

void SkiaRenderTarget::TileRect(RECT* lprc, IRenderBitmap* pRenderBitmap)
{
	UIASSERT(false);
}

void SkiaRenderTarget::Rectangle(RECT* lprc, ui::Color* pColBorder, ui::Color* pColBack, int nBorder, bool bNullBack)
{
	UIASSERT(false);
}

void SkiaRenderTarget::DrawFocusRect(RECT* lprc )
{
UIASSERT(false);
}
void SkiaRenderTarget::DrawLine(int x1, int y1, int x2, int y2, IRenderPen* pPen)
{
UIASSERT(false);
}
void SkiaRenderTarget::DrawPolyline(POINT* lppt, int nCount, IRenderPen* pPen)
{
UIASSERT(false);
}

void SkiaRenderTarget::GradientFillH(RECT* lprc, COLORREF colFrom, COLORREF colTo )
{
//	util::GradientFillH(GetHDC(), lprc, colFrom, colTo );
}
void SkiaRenderTarget::GradientFillV(RECT* lprc, COLORREF colFrom, COLORREF colTo )
{
//	util::GradientFillV(GetHDC(), lprc, colFrom, colTo );
}

void SkiaRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, unsigned int dwRop)
{UIASSERT(false);
}
 

void SkiaRenderTarget::ImageList_Draw( IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy )
{
  UIASSERT(false);
}

void SkiaRenderTarget::DrawBitmapEx(HDC hDC, IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam)
{
  UIASSERT(false);
}
void SkiaRenderTarget::DrawBitmap(IRenderBitmap* pRenderBitmap, 
                                 DRAWBITMAPPARAM* pParam)
{
  UIASSERT(false);
}


IRenderPen* SkiaRenderTarget::CreateSolidPen(int nWidth, Color* pColor)
{
	// IRenderPen* p = nullptr;
	// GdiPen::CreateInstance(&p);

	// if (p)
	// {
	// 	p->CreateSolidPen(nWidth, pColor);
	// }
	// return p;
  UIASSERT(false);
  return nullptr;
}
IRenderPen* SkiaRenderTarget::CreateDotPen(int nWidth, Color* pColor)
{
	// IRenderPen* p = nullptr;
	// GdiPen::CreateInstance(&p);

	// if (p)
	// {
	// 	p->CreateDotPen(nWidth, pColor);
	// }
	// return p;
  UIASSERT(false);
  return nullptr;
}
IRenderBrush*  SkiaRenderTarget::CreateSolidBrush(Color* pColor)
{
	// IRenderBrush* p = nullptr;
	// GdiBrush::CreateInstance(&p);

	// if (p)
	// {
	// 	p->CreateSolidBrush(pColor);
	// }
	// return p;
  UIASSERT(false);
  return nullptr;
}


void  SkiaRenderTarget::Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam)
{
}

void  SkiaRenderTarget::Save(const wchar_t*  szPath )
{
    // if (m_pRenderBuffer)
    // {
    //     m_pRenderBuffer->Dump(szPath);
    // }
}

void  SkiaRenderTarget::Upload2Gpu(IGpuRenderLayer* p, RECT* prcArray, int nCount)
{
    // if (m_pRenderBuffer)
    // {
    //     m_pRenderBuffer->Upload2Gpu(p, prcArray, nCount);
    // }
}

} // namespace ui