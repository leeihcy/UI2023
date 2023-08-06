#include "include/inc.h"
#include "skia_render.h"
#include "include/util/log.h"

namespace ui {

SkiaRenderTarget::SkiaRenderTarget(bool bNeedAlphaChannel) {
  m_bNeedAlphaChannel = bNeedAlphaChannel;
}

SkiaRenderTarget::~SkiaRenderTarget() {
  // SAFE_DELETE(m_pRenderBuffer);
  // m_hBindDC = nullptr;
}

void SkiaRenderTarget::Release() { delete this; }

// 该HDC不需要释放
// HDC SkiaRenderTarget::GetBindHDC()
// {
// 	return m_hBindDC;
// }

void SkiaRenderTarget::SetMetaClipRegion(RECT *prc, uint nrcCount) {}

void SkiaRenderTarget::PushRelativeClipRect(const RECT *prc) {
  UIASSERT(false);
}

void SkiaRenderTarget::PopRelativeClipRect() { UIASSERT(false); }

bool SkiaRenderTarget::IsRelativeRectInClip(const RECT *prc) {
  UIASSERT(false);
  return false;
}

void SkiaRenderTarget::SetOrigin(int x, int y) {
  // if (m_ptOffset.x == x && m_ptOffs-
}

void SkiaRenderTarget::OffsetOrigin(int x, int y) {
  // m_ptOffset.x += x;
  // m_ptOffset.y += y;

  // POINT pt = {0};
  // ::SetViewportOrgEx(GetHDC(), m_ptOffset.x, m_ptOffset.y, &pt);
}
void SkiaRenderTarget::GetOrigin(int *px, int *py) {
  // if (px)
  //     *px = m_ptOffset.x;

  // if (py)
  //     *py = m_ptOffset.y;
}

bool SkiaRenderTarget::CreateRenderBuffer(IRenderTarget *pSrcRT) {
  return true;
}

bool SkiaRenderTarget::ResizeRenderBuffer(unsigned int width,
                                          unsigned int height) {
  // if (m_sksurface) {
  //   return true;
  // }
  printf("ResizeRenderBuffer: %d %d\n", width, height);
  SkImageInfo info = SkImageInfo::Make(width, height, kBGRA_8888_SkColorType,
                                       kPremul_SkAlphaType);
  SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);

  m_sksurface = SkSurface::MakeRaster(info, &surfaceProps);


  // test，还没刷新界面，就直接commit的话，会出现全黄色。
   SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->clear(SK_ColorYELLOW);

  // m_sksurface = SkSurface::MakeRasterN32Premul(width, height);
  // SkCanvas *canvas = m_sksurface->getCanvas();
  return true;
}

void SkiaRenderTarget::GetRenderBufferData(ImageData *pData) {
  // if (!m_pRenderBuffer)
  // 	return;
  // m_pRenderBuffer->GetImageData(pData);
}

bool SkiaRenderTarget::BeginDraw() { return true; }

void SkiaRenderTarget::EndDraw() {
  // if (!m_sksurface) {
  //   return;
  // }

  // SkCanvas *canvas = m_sksurface->getCanvas();
  // on_erase_bkgnd(*canvas);
  // on_paint(*canvas);

  // m_sksurface->flushAndSubmit();
  // swap_buffer();
}

void SkiaRenderTarget::Clear(RECT *prc) {
  if (!m_sksurface) {
    UI_LOG_WARN(L"no sksurface");
    return;
  }
  SkCanvas *canvas = m_sksurface->getCanvas();
  SkColor colors[] = {SK_ColorRED, SK_ColorGREEN, SK_ColorBLUE};
  static int i= 0; 
  i++;
  if (i >= 3){ i = 0;}
  canvas->clear(colors[i]);
  printf("clear render \n");
  // UI_LOG_INFO(L"clear render");
}

void SkiaRenderTarget::DrawString(IRenderFont *pRenderFont,
                                  DRAWTEXTPARAM *pParam) {
  UIASSERT(false);
}

void SkiaRenderTarget::FillRgn(HRGN hRgn, ui::Color *pColor) {
  UIASSERT(false);
}

void SkiaRenderTarget::DrawRect(RECT *lprc, ui::Color *pColor) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkRect skrect;
  skrect.fLeft = lprc->left;
  skrect.fTop = lprc->top;
  skrect.fRight = lprc->right;
  skrect.fBottom = lprc->bottom;

  SkPaint paint;
  paint.setColor(SkColorSetARGB(pColor->a, pColor->r, pColor->g, pColor->b));
  canvas->drawRect(skrect, paint);
}

void SkiaRenderTarget::TileRect(RECT *lprc, IRenderBitmap *pRenderBitmap) {
  UIASSERT(false);
}

void SkiaRenderTarget::Rectangle(RECT *lprc, ui::Color *pColBorder,
                                 ui::Color *pColBack, int nBorder,
                                 bool bNullBack) {
  UIASSERT(false);
}

void SkiaRenderTarget::DrawFocusRect(RECT *lprc) { UIASSERT(false); }
void SkiaRenderTarget::DrawLine(int x1, int y1, int x2, int y2,
                                IRenderPen *pPen) {
  UIASSERT(false);
}
void SkiaRenderTarget::DrawPolyline(POINT *lppt, int nCount, IRenderPen *pPen) {
  UIASSERT(false);
}

void SkiaRenderTarget::GradientFillH(RECT *lprc, COLORREF colFrom,
                                     COLORREF colTo) {
  //	util::GradientFillH(GetHDC(), lprc, colFrom, colTo );
}
void SkiaRenderTarget::GradientFillV(RECT *lprc, COLORREF colFrom,
                                     COLORREF colTo) {
  //	util::GradientFillV(GetHDC(), lprc, colFrom, colTo );
}

void SkiaRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest,
                              IRenderTarget *pSrcHDC, int xSrc, int ySrc,
                              unsigned int dwRop) {
  UIASSERT(false);
}

void SkiaRenderTarget::ImageList_Draw(IRenderBitmap *hBitmap, int x, int y,
                                      int col, int row, int cx, int cy) {
  UIASSERT(false);
}

void SkiaRenderTarget::DrawBitmapEx(HDC hDC, IRenderBitmap *pBitmap,
                                    DRAWBITMAPPARAM *pParam) {
  UIASSERT(false);
}
void SkiaRenderTarget::DrawBitmap(IRenderBitmap *pRenderBitmap,
                                  DRAWBITMAPPARAM *pParam) {
  UIASSERT(false);
}

IRenderPen *SkiaRenderTarget::CreateSolidPen(int nWidth, Color *pColor) {
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
IRenderPen *SkiaRenderTarget::CreateDotPen(int nWidth, Color *pColor) {
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
IRenderBrush *SkiaRenderTarget::CreateSolidBrush(Color *pColor) {
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

void SkiaRenderTarget::Render2Target(IRenderTarget *pDst,
                                     Render2TargetParam *pParam) {}

void SkiaRenderTarget::Save(const wchar_t *szPath) {
  // if (m_pRenderBuffer)
  // {
  //     m_pRenderBuffer->Dump(szPath);
  // }
}

void SkiaRenderTarget::Upload2Gpu(IGpuRenderLayer *p, RECT *prcArray,
                                  int nCount) {
  // if (m_pRenderBuffer)
  // {
  //     m_pRenderBuffer->Upload2Gpu(p, prcArray, nCount);
  // }
}

} // namespace ui