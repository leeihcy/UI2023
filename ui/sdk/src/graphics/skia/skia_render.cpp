#include "skia_render.h"
#include "include/inc.h"
#include "include/util/log.h"

namespace ui {

void toSkRect(Rect &rc, SkRect *skrect) {
  skrect->fLeft = rc.left;
  skrect->fTop = rc.top;
  skrect->fRight = rc.right;
  skrect->fBottom = rc.bottom;
}

SkiaRenderTarget::SkiaRenderTarget(bool bNeedAlphaChannel) {
  m_bNeedAlphaChannel = bNeedAlphaChannel;
  m_ptOffset.x = m_ptOffset.y = 0;
}

SkiaRenderTarget::~SkiaRenderTarget() {
  // SAFE_DELETE(m_pRenderBuffer);
  // m_hBindDC = nullptr;
}

void SkiaRenderTarget::Release() { delete this; }

void SkiaRenderTarget::update_clip_rgn() {
  SkCanvas *canvas = m_sksurface->getCanvas();

  int count = m_arrayMetaClipRegion.GetCount();
  for (int i = 0; i < count; i++) {
    Rect *prc = m_arrayMetaClipRegion.GetRectPtrAt(i);

    SkRect skrc;
    toSkRect(*prc, &skrc);

    canvas->clipRect(skrc);
  }

  if (!m_stackClipRect.empty()) {
    auto iter = m_stackClipRect.begin();

    Rect rcIntersect = *iter;
    iter++;
    
    for (; iter != m_stackClipRect.end(); ++iter) {
      rcIntersect.Intersect(*iter, &rcIntersect);
    }

    SkRect skrc;
    toSkRect(rcIntersect, &skrc);
    canvas->clipRect(skrc);
  }
}

void SkiaRenderTarget::SetMetaClipRegion(Rect *prc, uint nrcCount) {
  // while (!m_stackClipRect.empty())
  //   m_stackClipRect.pop();
  m_stackClipRect.clear();

  m_arrayMetaClipRegion.CopyFromArray(prc, nrcCount);

  update_clip_rgn();
}

void SkiaRenderTarget::PushRelativeClipRect(const Rect *prc) {
  Rect rc = {0};
  if (prc) {
    rc.CopyFrom(*prc);
    rc.Offset(m_ptOffset.x, m_ptOffset.y);
  }

  m_stackClipRect.push_back(rc);

  update_clip_rgn();
}

void SkiaRenderTarget::PopRelativeClipRect() {
  assert(!m_stackClipRect.empty());
  m_stackClipRect.pop_back();

  update_clip_rgn();
}

bool SkiaRenderTarget::IsRelativeRectInClip(const Rect *prc) {
  assert(prc);
  if (!prc)
    return false;

  if (m_stackClipRect.empty() && m_arrayMetaClipRegion.GetCount() == 0)
    return true;

  Rect rcTest = *prc;
  rcTest.Offset(m_ptOffset.x, m_ptOffset.y);

  if (!m_arrayMetaClipRegion.IntersectRect(&rcTest, true))
    return false;

  if (m_stackClipRect.empty())
    return true;

  Rect rcIntersect = {0, 0, 1, 1};

  auto iter = m_stackClipRect.begin();
  rcIntersect.CopyFrom(*iter);
  iter++;
  for (; iter != m_stackClipRect.end(); ++iter) {
    rcIntersect.Intersect((*iter), &rcIntersect);
  }

  if (!rcIntersect.Intersect(rcTest, &rcIntersect))
    return false;

  return true;
}

void SkiaRenderTarget::SetOrigin(int x, int y) {
  if (m_ptOffset.x == x && m_ptOffset.y == y)
    return;

  int offsetx = x - m_ptOffset.x;
  int offsety = y - m_ptOffset.y;
  m_ptOffset.x = x;
  m_ptOffset.y = y;

  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->translate(offsetx, offsety);
}
void SkiaRenderTarget::OffsetOrigin(int x, int y) {
  m_ptOffset.x += x;
  m_ptOffset.y += y;

  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->translate(x, y);
}
void SkiaRenderTarget::GetOrigin(int *px, int *py) {
  if (px)
    *px = m_ptOffset.x;

  if (py)
    *py = m_ptOffset.y;
}

bool SkiaRenderTarget::CreateRenderBuffer(IRenderTarget *pSrcRT) {
  return true;
}

bool SkiaRenderTarget::ResizeRenderBuffer(unsigned int width,
                                          unsigned int height) {
  // if (m_sksurface) {
  //   return true;
  // }
  SkImageInfo info = SkImageInfo::Make(width, height, kBGRA_8888_SkColorType,
                                       kPremul_SkAlphaType);
  SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);

  m_sksurface = SkSurface::MakeRaster(info, &surfaceProps);

  // test，还没刷新界面，就直接commit的话，会出现全黄色。
#if defined(DEBUG)
  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->clear(SK_ColorYELLOW);
#endif
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

void SkiaRenderTarget::Clear(Rect *prc) {
  if (!m_sksurface) {
    UI_LOG_WARN(L"no sksurface");
    return;
  }
  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->clear(SK_ColorTRANSPARENT);
}

void SkiaRenderTarget::DrawString(IRenderFont *pRenderFont,
                                  DRAWTEXTPARAM *pParam) {
  UIASSERT(false);
}

void SkiaRenderTarget::FillRgn(HRGN hRgn, ui::Color *pColor) {
  UIASSERT(false);
}

void SkiaRenderTarget::DrawRect(Rect *lprc, ui::Color *pColor) {
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

void SkiaRenderTarget::TileRect(Rect *lprc, IRenderBitmap *pRenderBitmap) {
  UIASSERT(false);
}

void SkiaRenderTarget::Rectangle(Rect *lprc, ui::Color *pColBorder,
                                 ui::Color *pColBack, int nBorder,
                                 bool bNullBack) {
  UIASSERT(false);
}

void SkiaRenderTarget::DrawFocusRect(Rect *lprc) { UIASSERT(false); }
void SkiaRenderTarget::DrawLine(int x1, int y1, int x2, int y2,
                                IRenderPen *pPen) {
  UIASSERT(false);
}
void SkiaRenderTarget::DrawPolyline(Point *lppt, int nCount, IRenderPen *pPen) {
  UIASSERT(false);
}

void SkiaRenderTarget::GradientFillH(Rect *lprc, Color colFrom, Color colTo) {
  //	util::GradientFillH(GetHDC(), lprc, colFrom, colTo );
}
void SkiaRenderTarget::GradientFillV(Rect *lprc, Color colFrom, Color colTo) {
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

void SkiaRenderTarget::Save(const char *szPath) {
  // if (m_pRenderBuffer)
  // {
  //     m_pRenderBuffer->Dump(szPath);
  // }
}

void SkiaRenderTarget::Upload2Gpu(IGpuRenderLayer *p, Rect *prcArray,
                                  int nCount) {
  // if (m_pRenderBuffer)
  // {
  //     m_pRenderBuffer->Upload2Gpu(p, prcArray, nCount);
  // }
}

} // namespace ui