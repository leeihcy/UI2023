#include "skia_render.h"
#include "include/inc.h"
#include "include/util/log.h"

#include "SkStream.h"
#if defined(OS_WIN)
#include "src/util/windows.h"
#endif

namespace ui {

void toSkRect(Rect &rc, SkRect *skrect) {
  skrect->fLeft = (SkScalar)rc.left;
  skrect->fTop = (SkScalar)rc.top;
  skrect->fRight = (SkScalar)rc.right;
  skrect->fBottom = (SkScalar)rc.bottom;
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
  canvas->translate((SkScalar)offsetx, (SkScalar)offsety);
}
void SkiaRenderTarget::OffsetOrigin(int x, int y) {
  m_ptOffset.x += x;
  m_ptOffset.y += y;

  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->translate((SkScalar)x, (SkScalar)y);
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

// 为了加快鼠标拖拽窗口的Resize效率
bool SkiaRenderTarget::ResizeRenderBuffer(unsigned int width,
                                          unsigned int height) {
  // 256的倍数，并且不减
  int fix_width = width;
  if ((fix_width & 0xFF) != 0) {
    fix_width = (fix_width & 0xFFFFFF00) + 0x100;
  }
  int fix_height = height;
  if ((fix_height & 0xFF) != 0) {
    fix_height = (fix_height & 0xFFFFFF00) + 0x100;
  }

  if (m_sksurface) {
    if (m_sksurface->width() >= fix_width && m_sksurface->height() >= fix_height) {
      return true;
    }
  }

  UI_LOG_INFO("SkiaRenderTarget resize:%d,%d => %d,%d(%x, %x)", 
    width, height, fix_width, fix_height, fix_width, fix_height);

  SkImageInfo info = SkImageInfo::Make(fix_width, fix_height, kBGRA_8888_SkColorType,
                                       kPremul_SkAlphaType);
  SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);

  m_sksurface = SkSurface::MakeRaster(info, &surfaceProps);

  // test，还没刷新界面，就直接commit的话，会出现全黄色。
// #if defined(DEBUG)
//   SkCanvas *canvas = m_sksurface->getCanvas();
//   canvas->clear(SK_ColorYELLOW);
// #endif
  // m_sksurface = SkSurface::MakeRasterN32Premul(width, height);
  // SkCanvas *canvas = m_sksurface->getCanvas();
  return true;
}

void SkiaRenderTarget::GetRenderBufferData(ImageData *pData) {
  // if (!m_pRenderBuffer)
  // 	return;
  // m_pRenderBuffer->GetImageData(pData);
}

bool SkiaRenderTarget::BeginDraw() { 
  if (!m_sksurface) {
    return false;
  }

  SkCanvas *canvas = m_sksurface->getCanvas();
  if (!canvas) {
    return false;
  }
  canvas->save();
  return true; 
}

void SkiaRenderTarget::EndDraw() {
if (!m_sksurface) {
    return;
  }

  SkCanvas *canvas = m_sksurface->getCanvas();
  if (!canvas) {
    return;
  }
  canvas->restore();
  
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

void SkiaRenderTarget::FillRgn(/*HRGN*/ llong hRgn, ui::Color *pColor) {
  UIASSERT(false);
}

void SkiaRenderTarget::DrawRect(Rect *lprc, ui::Color *pColor) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkRect skrect;
  skrect.fLeft = (SkScalar)lprc->left;
  skrect.fTop = (SkScalar)lprc->top;
  skrect.fRight = (SkScalar)lprc->right;
  skrect.fBottom = (SkScalar)lprc->bottom;

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

void SkiaRenderTarget::DrawBitmapEx(/*HDC*/ llong hDC, IRenderBitmap *pBitmap,
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

#if defined(OS_WIN)
void SkiaRenderTarget::Render2DC(/*HDC*/ llong _hDC,
                                 Render2TargetParam *pParam) {
  if (!m_sksurface) {
    return;
  }
  HDC hDC = (HDC)_hDC;

  int &xDst = pParam->xDst;
  int &yDst = pParam->yDst;
  int &wDst = pParam->wDst;
  int &hDst = pParam->hDst;
  int &xSrc = pParam->xSrc;
  int &ySrc = pParam->ySrc;
  int &wSrc = pParam->wSrc;
  int &hSrc = pParam->hSrc;
  bool &bAlphaBlend = pParam->bAlphaBlend;
  byte &opacity = pParam->opacity;

  //  HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
  //  RECT rc = {xDst, yDst, xDst + wDst, yDst + hDst};
  // ::FillRect(hDC, &rc, hBrush);

  SkPixmap pm;
  if (!m_sksurface->peekPixels(&pm)) {
    return;
  }

  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(bmi));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = pm.width();
  bmi.bmiHeader.biHeight = -pm.height(); // top-down image
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;

  // if (wDst == wSrc && hDst == hSrc) {
  //   SetDIBitsToDevice
  // }
  ::StretchDIBits(hDC, xDst, ySrc, wDst, hDst, xSrc, ySrc, wSrc, hSrc,
                  pm.addr(), &bmi, DIB_RGB_COLORS, SRCCOPY);

  // if (bAlphaBlend) {
  //   BLENDFUNCTION bf = {AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA};
  //   ::AlphaBlend(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, wSrc, hSrc,
  //                bf);
  // } else {
  //   if (wDst == wSrc && hDst == hSrc) {
  //     ::BitBlt(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, SRCCOPY);
  //   } else {
  //     ::StretchBlt(hDstDC, xDst, ySrc, wDst, hDst, hDC, xSrc, ySrc, wSrc,
  //     hSrc,
  //                  SRCCOPY);
  //   }
  // }
}
#endif

void SkiaRenderTarget::Render2Target(IRenderTarget *pDst,
                                     Render2TargetParam *pParam) {}

void SkiaRenderTarget::Save(const char *path) {
  if (!m_sksurface) {
    return;
  }
  // if (m_pRenderBuffer)
  //     m_pRenderBuffer->Dump(szPath);
  sk_sp<SkImage> img(m_sksurface->makeImageSnapshot());
  if (!img) {
    return;
  }
  sk_sp<SkData> png(img->encodeToData());
  if (!png) {
    return;
  }
  SkFILEWStream out(path);
  (void)out.write(png->data(), png->size());
}

void SkiaRenderTarget::Upload2Gpu(IGpuRenderLayer *p, Rect *prcArray,
                                  int nCount) {
  // if (m_pRenderBuffer)
  // {
  //     m_pRenderBuffer->Upload2Gpu(p, prcArray, nCount);
  // }
}

} // namespace ui