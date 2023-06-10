#pragma once

#include "gdibitmap.h"
#include "gdifont.h"
#include "gdipen.h"
#include "gdibrush.h"
#include "Src\Util\RectArray\rectarray.h"

namespace UI
{
class RenderBuffer;

class GdiRenderTarget : public IRenderTarget
{
public:
 	GdiRenderTarget(bool bNeedAlphaChannel);
	virtual ~GdiRenderTarget();
    virtual void Release();
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }
    virtual bool  IsRenderAlphaChannel() { return m_bNeedAlphaChannel; }

    virtual HDC   GetHDC();
    virtual void  ReleaseHDC(HDC hDC);
	virtual void  BindHDC(HDC hDC);
	virtual HDC   GetBindHDC();
    virtual bool  CreateRenderBuffer(IRenderTarget*  pSrcRT);
    virtual bool  ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight);
	virtual void  GetRenderBufferData(ImageData*  pData);
    virtual void  BindHWND(HWND hWnd) { /*UIASSERT(0);*/ /*不支持*/ }

    virtual void  SetMetaClipRegion(LPRECT prc, uint nrcCount) override;
    virtual void  PushRelativeClipRect(LPCRECT) override;
    virtual void  PopRelativeClipRect() override;
    virtual bool  IsRelativeRectInClip(LPCRECT) override;
    virtual void  SetOrigin(int x, int y) override;
    virtual void  OffsetOrigin(int x, int y) override;
    virtual void  GetOrigin(int* px, int* py) override;

	virtual bool  BeginDraw();
	virtual void  EndDraw( );
	virtual void  Clear(RECT* prc);
	virtual void  Save(const TCHAR*  szPath );
    virtual void  Render2DC(HDC hDC, Render2TargetParam* pParam);
    virtual void  Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam);

	virtual void  FillRgn(HRGN, UI::Color* pColor);
	virtual void  DrawRect(LPRECT lprc, UI::Color* pColor);
	virtual void  TileRect(LPRECT lprc, IRenderBitmap* hBitmap);
	virtual void  Rectangle(LPRECT lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack);
	virtual void  DrawFocusRect(LPRECT lprc );
	virtual void  DrawLine(int x1, int y1, int x2, int y2, IRenderPen*);
	virtual void  DrawPolyline(POINT* lppt, int nCount, IRenderPen*);
	virtual void  GradientFillH(LPRECT lprc, COLORREF colFrom, COLORREF colTo);
	virtual void  GradientFillV(LPRECT lprc, COLORREF colFrom, COLORREF colTo);
	virtual void  BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop);
	virtual void  ImageList_Draw(IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy );
	virtual void  DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam);
//	virtual void  DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree, DRAWBITMAPPARAM* pParam);
    virtual void  DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam);
	static  void  DrawBitmapEx(HDC hDC, IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam);

	virtual IRenderPen*     CreateSolidPen(int nWidth, Color* pColor);
	virtual IRenderPen*     CreateDotPen(int nWidth, Color* pColor);
	virtual IRenderBrush*   CreateSolidBrush(Color* pColor);

    virtual void  Upload2Gpu(IGpuRenderLayer* p, LPRECT prcArray, int nCount);

protected:
	void  DrawBitmap(IRenderBitmap* hBitmap, int x, int y);
	void  DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	void  DrawBitmap(IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
						int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
	void  DrawBitmap(IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
						int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
						C9Region* p9Region );

    void  draw_string_normal(HFONT hFont, LPRECT prcText, DRAWTEXTPARAM* pParam);
    void  draw_string_halo(HFONT hFont, const CRect& rcText, DRAWTEXTPARAM* pParam);
    void  draw_string_endalphamask(HFONT hFont, LPRECT prcText, DRAWTEXTPARAM* pParam);

    void  FixAlpha0To255(HDC hDC, LPCRECT lpRect);
    void  FillAlpha255(HDC hDC, LPCRECT lpRect);
	void  FillAlphaValue(HDC hDC, LPCRECT lpRect, WPARAM);
    void  InverseAlpha(HDC hDC, LPCRECT lpRect);
    void  _FixAlpha(HDC hDC, LPCRECT lpRect, Util::FixAlphaMode e, WPARAM wParam);


    void  update_clip_rgn();

protected:
    HDC   m_hBindDC;
    RenderBuffer*  m_pRenderBuffer;
    bool  m_bNeedAlphaChannel;  // 需要渲染alpha通道

	long  m_lDrawingRef; // 标识外部调用了几次BeginDraw，解决嵌套调用出现的一些问题

    // 剪裁数据
    RectArray  m_arrayMetaClipRegion;
    stack<RECT>  m_stackClipRect;

    POINT  m_ptOffset;   // 用于调试时查看当前HDC偏移量
};

}

