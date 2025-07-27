#pragma once
#include <vector>

#include <SkCanvas.h>
#include <SkSurface.h>
#include <SkColorSpace.h>

#include "include/interface/renderlibrary.h"
#include "src/util/rectregion/rectregion.h"

// #include "Src\Util\RectRegion\rectarray.h"
// #include "gdibitmap.h"
// #include "gdibrush.h"
// #include "gdifont.h"
// #include "gdipen.h"

namespace ui {
class RenderBuffer;

class SkiaRenderTarget : public IRenderTarget {
public:
  SkiaRenderTarget(bool bNeedAlphaChannel);
  virtual ~SkiaRenderTarget();
  virtual void Release() override;
  virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() override {
    return GRAPHICS_RENDER_LIBRARY_TYPE_SKIA;
  }
  virtual bool IsRenderAlphaChannel() override { return m_bNeedAlphaChannel; }
  virtual void* GetHandle() override { return (void*)m_sksurface->getCanvas(); }
  // virtual HDC GetHDC();
  // virtual void ReleaseHDC(HDC hDC);
  // virtual void BindHDC(HDC hDC);
  // virtual HDC GetBindHDC();
  virtual bool CreateRenderBuffer(IRenderTarget *pSrcRT) override;
  virtual bool ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight) override;
  virtual void GetRenderBufferData(ImageData *pData) override;
  // virtual void BindHWND(HWND hWnd) { /*UIASSERT(0);*/ /*不支持*/}

  virtual void SetMetaClipRegion(Rect* prc, uint nrcCount) override;
  virtual void PushRelativeClipRect(const Rect*) override;
  virtual void PopRelativeClipRect() override;
  virtual bool IsRelativeRectInClip(const Rect*) override;
  virtual void SetOrigin(int x, int y) override;
  virtual void OffsetOrigin(int x, int y) override;
  virtual void GetOrigin(int *px, int *py) override;

  virtual bool BeginDraw(float scale) override;
  virtual void EndDraw() override;
  virtual void Clear(Rect *prc) override;
  virtual void Save(const char *szPath) override;
#if defined(OS_WIN)  
  virtual void Render2DC(/*HDC*/llong hDC, Render2TargetParam *pParam) override;
#endif
  virtual void Render2Target(IRenderTarget *pDst, Render2TargetParam *pParam) override;

  virtual void FillRgn(/*HRGN*/llong, ui::Color *pColor) override;
  virtual void DrawRect(Rect* lprc, ui::Color *pColor) override;
  virtual void TileRect(Rect* lprc, IRenderBitmap *hBitmap) override;
  virtual void Rectangle(Rect* lprc, ui::Color *pColBorder,
                         ui::Color *pColBack, int nBorder, bool bNullBack) override;
  virtual void DrawFocusRect(Rect* lprc) override;
  virtual void DrawLine(int x1, int y1, int x2, int y2, IRenderPen *) override;
  virtual void DrawPolyline(Point *lppt, int nCount, IRenderPen *) override;
  virtual void GradientFillH(Rect* lprc, Color colFrom, Color colTo) override;
  virtual void GradientFillV(Rect* lprc, Color colFrom, Color colTo) override;
  virtual void BitBlt(int xDest, int yDest, int wDest, int hDest,
                      IRenderTarget *pSrcHDC, int xSrc, int ySrc, unsigned int dwRop) override;
  virtual void ImageList_Draw(IRenderBitmap *hBitmap, int x, int y, int col,
                              int row, int cx, int cy) override;
  virtual void DrawBitmap(IRenderBitmap *hBitmap, DRAWBITMAPPARAM *pParam) override;
  //	virtual void  DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree,
  //DRAWBITMAPPARAM* pParam);
  virtual void DrawString(const DrawTextParam& param) override;

  virtual IRenderPen *CreateSolidPen(int nWidth, Color *pColor) override;
  virtual IRenderPen *CreateDotPen(int nWidth, Color *pColor) override;
  virtual IRenderBrush *CreateSolidBrush(Color *pColor) override;

  virtual void Upload2Gpu(IGpuLayer *p, Rect* prcArray, int nCount) override;

public:
  SkSurface* GetSkiaSurface() { return m_sksurface.get(); }

protected:
  // void DrawBitmap(IRenderBitmap *hBitmap, int x, int y);
  // void DrawBitmap(IRenderBitmap *pBitmap, int xDest, int yDest, int wDest,
  //                 int hDest, int xSrc, int ySrc);
  // void DrawBitmap(IRenderBitmap *hBitmap, int xDest, int yDest, int nDestWidth,
  //                 int nDestHeight, int xSrc, int ySrc, int nSrcWidth,
  //                 int nSrcHeight);
  // void DrawBitmap(IRenderBitmap *hBitmap, int xDest, int yDest, int nDestWidth,
  //                 int nDestHeight, int xSrc, int ySrc, int nSrcWidth,
  //                 int nSrcHeight, C9Region *p9Region);

  // void draw_string_normal(HFONT hFont, Rect* prcText, DRAWTEXTPARAM *pParam);
  // void draw_string_halo(HFONT hFont, const Rect &rcText,
  //                       DRAWTEXTPARAM *pParam);
  // void draw_string_endalphamask(HFONT hFont, Rect* prcText,
  //                               DRAWTEXTPARAM *pParam);

  // void FixAlpha0To255(HDC hDC, const Rect* lpRect);
  // void FillAlpha255(HDC hDC, const Rect* lpRect);
  // void FillAlphaValue(HDC hDC, const Rect* lpRect, long);
  // void InverseAlpha(HDC hDC, const Rect* lpRect);
  // void _FixAlpha(HDC hDC, const Rect* lpRect, util::FixAlphaMode e, long wParam);

  void update_clip_rgn();

protected:
  // 渲染
  // SkBitmap m_bitmap;

  sk_sp<SkSurface> m_sksurface;

  float m_scale = 1.0f;

  // HDC m_hBindDC;
  // RenderBuffer *m_pRenderBuffer;
  bool m_bNeedAlphaChannel; // 需要渲染alpha通道

  // long m_lDrawingRef; // 标识外部调用了几次BeginDraw，解决嵌套调用出现的一些问题

  // 剪裁数据
  RectRegion m_arrayMetaClipRegion;
  std::vector<Rect> m_stackClipRect;  // 需要遍历，不使用std::stack

  Point m_ptOffset; // 用于调试时查看当前HDC偏移量
};

} // namespace ui
