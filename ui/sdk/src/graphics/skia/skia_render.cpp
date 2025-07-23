#include "skia_render.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkRect.h"
#include "include/inc.h"
#include "include/macro/xmldefine.h"
#include "include/util/log.h"
#include "skia_bitmap.h"

#include "SkStream.h"
#include "gpu/include/api.h"
#include <cassert>

#include "third_party/skia/src/include/core/SkBitmap.h"
#include "third_party/skia/src/include/core/SkFont.h"

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
  // TODO: canvas->clipRect 不支持replace操作，导致clip区域越来越小，无法还原
  // 目前好像只能通过 save/restore 来还原clip区域，太恶心了。先不支持clip吧。
#if 0
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
#endif
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
    if (m_sksurface->width() >= fix_width &&
        m_sksurface->height() >= fix_height) {
      return true;
    }
  }

  UI_LOG_INFO("SkiaRenderTarget resize:%d,%d => %d,%d(0x%x, 0x%x)", width,
              height, fix_width, fix_height, fix_width, fix_height);

  SkImageInfo info = SkImageInfo::Make(
      fix_width, fix_height, kBGRA_8888_SkColorType, kPremul_SkAlphaType);
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
  if (!m_sksurface || !prc) {
    UI_LOG_WARN(L"no sksurface");
    return;
  }
  SkCanvas *canvas = m_sksurface->getCanvas();
  // canvas->clear(SK_ColorTRANSPARENT);  // <<-- 全量刷新非常影响性能

  SkRect skrect;
  skrect.fLeft = (SkScalar)prc->left;
  skrect.fTop = (SkScalar)prc->top;
  skrect.fRight = (SkScalar)prc->right;
  skrect.fBottom = (SkScalar)prc->bottom;

  SkPaint paint;
  paint.setColor(SK_ColorTRANSPARENT);
  paint.setBlendMode(SkBlendMode::kSrc);
  canvas->drawRect(skrect, paint);
}

void SkiaRenderTarget::DrawString(IRenderFont *pRenderFont,
                                  DrawTextParam *param) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  // Draw a message with a nice black paint
  SkFont font(SkTypeface::MakeFromName("monospace", SkFontStyle()), 10);
  // font.setSubpixel(true);
  // font.setSize(100);

  SkPaint paint;
  paint.setColor(SK_ColorBLACK);

  SkRect textBounds;
  font.measureText(param->text, strlen(param->text), SkTextEncoding::kUTF8,
  &textBounds, &paint);
  
  SkScalar padding = 2.0f;
  SkRect bg_rect = SkRect::MakeXYWH(
    param->bound.left + textBounds.fLeft - padding,
    param->bound.top + textBounds.fTop - padding,
    textBounds.width() + 2*padding,
    textBounds.height() + 2*padding
  );

  SkPaint bg_paint;
  bg_paint.setColor(SK_ColorYELLOW);
  canvas->drawRect(bg_rect, bg_paint);
  canvas->drawString(param->text, param->bound.left, param->bound.top, font, paint);
  // canvas->drawSimpleText(param->text, strlen(param->text),
  //                        SkTextEncoding::kUTF8, param->bound.left,
  //                        param->bound.top, font, paint);
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

void SkiaRenderTarget::DrawBitmap(IRenderBitmap *pRenderBitmap,
                                  DRAWBITMAPPARAM *pParam) {
  if (NULL == pRenderBitmap || NULL == pParam)
    return;

  if (pRenderBitmap->GetGraphicsRenderLibraryType() !=
      GRAPHICS_RENDER_LIBRARY_TYPE_SKIA) {
    return;
  }

  SkiaRenderBitmap *skia_bitmap =
      static_cast<SkiaRenderBitmap *>(pRenderBitmap);
  SkCanvas *canvas = m_sksurface->getCanvas();

  if (pParam->nFlag & DRAW_BITMAP_DISABLE) {
    // 	const Gdiplus::REAL r = (Gdiplus::REAL)0.3;
    // 	const Gdiplus::REAL g = (Gdiplus::REAL)0.59;
    // 	const Gdiplus::REAL b = (Gdiplus::REAL)0.11;
    // 	const Gdiplus::ColorMatrix matrix =
    // 			{r,    r,   r,     0.0f,  0.0f,      // Red
    // 			 g,    g,    g,    0.0f,  0.0f,      // Green
    // 			 b,    b,    b,    0.0f,  0.0f,      // Blue
    // 			 0.0f, 0.0f, 0.0f, fAlphaBlend,  0.0f,      // Alpha
    // 			 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};
    // 	imageAttribute.SetColorMatrix(&matrix);
    // 	pImageAttribute = &imageAttribute;
  } else if (pParam->nAlpha != 255) {
    // 	const Gdiplus::ColorMatrix matrix =
    // 			{1.0f, 0.0f, 0.0f,  0.0f,  0.0f,      // Red
    // 			 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,      // Green
    // 			 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,      // Blue
    // 			 0.0f, 0.0f, 0.0f, fAlphaBlend,  0.0f,      // Alpha
    // 			 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};
    // 	imageAttribute.SetColorMatrix(&matrix);
    // 	pImageAttribute = &imageAttribute;
  }

  if (pParam->nFlag & DRAW_BITMAP_BITBLT) {

    int src_width = pParam->wSrc;
    int src_height = pParam->hSrc;

    int dest_width = pParam->wDest;
    int dest_height = pParam->hDest;

    // 处理DPI缩放
    //if (pParam->scale_factor == 0 || pParam->scale_factor == 1.0) {
    //  src_width = dest_width = std::min(pParam->wDest, src_width);
    //  src_height = dest_height = std::min(pParam->hDest, src_height);
    //} else {
    //  int scaled_src_width = pParam->wSrc/* * pParam->scale_factor */;
    //  int scaled_src_height = pParam->hSrc/* * pParam->scale_factor */;
    //  dest_width = std::min(pParam->wDest, scaled_src_width);
    //  dest_height = std::min(pParam->hDest, scaled_src_height);
    //
    //  if (dest_width < scaled_src_width) {
    //    src_width = dest_width / pParam->scale_factor;
    //  }
    //  if (dest_height < scaled_src_height) {
    //    src_height = dest_height / pParam->scale_factor;
    //  }
    //}

    SkSamplingOptions options;
    SkPaint paint;
    canvas->drawImageRect(
        skia_bitmap->m_bitmap.asImage(),
        SkRect::MakeXYWH(pParam->xSrc, pParam->ySrc, src_width, src_height),
        SkRect::MakeXYWH(pParam->xDest, pParam->yDest, dest_width, dest_height),
        options, &paint, SkCanvas::kFast_SrcRectConstraint);

    if (pParam->prcRealDraw) {
      pParam->prcRealDraw->Set(pParam->xDest, pParam->yDest,
                               pParam->xDest + dest_width,
                               pParam->yDest + dest_height);
    }
  } else if (pParam->nFlag & DRAW_BITMAP_STRETCH) {
    SkSamplingOptions options;
    SkPaint paint;
    canvas->drawImageRect(skia_bitmap->m_bitmap.asImage(),
                          SkRect::MakeXYWH(pParam->xSrc, pParam->ySrc,
                                           pParam->wSrc, pParam->hSrc),
                          SkRect::MakeXYWH(pParam->xDest, pParam->yDest,
                                           pParam->wDest, pParam->hDest),
                          options, &paint, SkCanvas::kFast_SrcRectConstraint);

    if (pParam->prcRealDraw) {
      pParam->prcRealDraw->Set(pParam->xDest, pParam->yDest,
                               pParam->xDest + pParam->wDest,
                               pParam->yDest + pParam->hDest);
    }
  }
  // else if (pParam->nFlag & DRAW_BITMAP_STRETCH_BORDER)
  // {
  // 	GdiplusRenderTarget::DrawBitmap(&g, p, pParam->xDest, pParam->yDest,
  // pParam->wDest, pParam->hDest, 		pParam->xSrc, pParam->ySrc,
  // pParam->wSrc, pParam->hSrc, pParam->pRegion, false, pImageAttribute);

  //       if (pParam->prcRealDraw)
  //       {
  //           pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest,
  //           pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
  //       }
  // }
  // else if (pParam->nFlag & DRAW_BITMAP_TILE)
  // {
  // 	Gdiplus::TextureBrush brush(pBitmap);
  // 	g.FillRectangle(&brush, pParam->xDest, pParam->yDest, pParam->wDest,
  // pParam->hDest);

  //       if (pParam->prcRealDraw)
  //       {
  //           pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest,
  //           pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
  //       }
  // }
  // else if (pParam->nFlag & DRAW_BITMAP_CENTER)
  // {
  // 	int x = pParam->xDest + (pParam->wDest - pParam->wSrc)/2;
  // 	int y = pParam->yDest + (pParam->hDest - pParam->hSrc)/2;

  // 	Gdiplus::RectF destRect((Gdiplus::REAL)x, (Gdiplus::REAL)y,
  // (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc);
  // 	g.DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc,
  // (Gdiplus::REAL)pParam->ySrc, (Gdiplus::REAL)pParam->wSrc,
  // (Gdiplus::REAL)pParam->hSrc, Gdiplus::UnitPixel, pImageAttribute, NULL,
  // NULL);

  //       if (pParam->prcRealDraw)
  //       {
  //           pParam->prcRealDraw->SetRect(x, y, x+pParam->wSrc,
  //           y+pParam->hSrc);
  //       }
  // }
  // else if (pParam->nFlag & DRAW_BITMAP_ADAPT)
  // {
  // 	if (pParam->wSrc == 0 || pParam->hSrc == 0)
  // 		return;

  // 	if (pParam->wDest == 0 || pParam->hDest == 0)
  // 		return;

  // 	bool bNeedToStretch = false;
  // 	int  wImage = pParam->wSrc;
  // 	int  hImage = pParam->hSrc;

  // 	if (pParam->wDest < pParam->wSrc || pParam->hDest < pParam->hSrc)
  // 	{
  // 		bNeedToStretch = true;

  // 		double tan_x_y_image = (double)pParam->wSrc /
  // (double)pParam->hSrc; 		double tan_x_y_dest =
  // (double)pParam->wDest / (double)pParam->hDest;

  // 		if (tan_x_y_image > tan_x_y_dest) // 横向占满
  // 		{
  // 			wImage = pParam->wDest;
  // 			hImage = (int)((double)wImage/tan_x_y_image);
  // 		}
  // 		else   // 纵向占满
  // 		{
  // 			hImage = pParam->hDest;
  // 			wImage = (int)(hImage*tan_x_y_image);
  // 		}
  // 	}

  // 	// 计算图片显示位置
  // 	int xDisplayPos = pParam->xDest + (pParam->wDest-wImage)/2;
  // 	int yDisplayPos = pParam->yDest + (pParam->hDest-hImage)/2;

  // 	if (bNeedToStretch)
  // 	{
  // 		GdiplusRenderTarget::DrawBitmap(&g, p, xDisplayPos, yDisplayPos,
  // wImage, hImage, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc,
  // pParam->pRegion, true, pImageAttribute);
  // 	}
  // 	else
  // 	{
  // 		Gdiplus::RectF destRect((Gdiplus::REAL)xDisplayPos,
  // (Gdiplus::REAL)yDisplayPos, (Gdiplus::REAL)pParam->wSrc,
  // (Gdiplus::REAL)pParam->hSrc); 		g.DrawImage(pBitmap, destRect,
  // (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc,
  // (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc,
  // Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);
  // 	}

  //       if (pParam->prcRealDraw)
  //       {
  //           pParam->prcRealDraw->SetRect(xDisplayPos, yDisplayPos,
  //           xDisplayPos+wImage, yDisplayPos+hImage);
  //       }
  // }
  //   else if (pParam->nFlag & DRAW_BITMAP_BITBLT_RIGHTTOP)
  //   {
  //       Gdiplus::REAL nW = (Gdiplus::REAL)min(pParam->wSrc,pParam->wDest);
  //       Gdiplus::REAL nH = (Gdiplus::REAL)min(pParam->hSrc,pParam->hDest);

  //       int xDest = pParam->xDest + (pParam->wDest-(int)nW);
  //       // int xSrc = pParam->xSrc+ (pParam->wSrc-(int)nW);

  //       Gdiplus::RectF destRect((Gdiplus::REAL)xDest,
  //       (Gdiplus::REAL)pParam->yDest, nW, nH); g.DrawImage(pBitmap, destRect,
  //       (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, nW, nH,
  //       Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);

  //       if (pParam->prcRealDraw)
  //       {
  //           pParam->prcRealDraw->SetRect(xDest, pParam->yDest, xDest+(int)nW,
  //           pParam->yDest+(int)nH);
  //       }
  //   }
  //   else if (pParam->nFlag & DRAW_BITMAP_STRETCH_ROUNDCORNER)
  //   {
  //       HDC hDC = Image::GetCacheDC();
  //       HBITMAP hCacheBitmap =
  //       CacheBitmap::GetInstance()->Create(pParam->wDest, pParam->hDest);
  //       HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, hCacheBitmap);

  //       // 绘制到临时位置
  //       {
  //           Gdiplus::Graphics g(hDC);
  // 		InitGraphics(g);

  //           Gdiplus::ImageAttributes attr;
  //           if (!pImageAttribute)
  //               pImageAttribute = &attr;
  //           pImageAttribute->SetWrapMode(Gdiplus::WrapModeTileFlipXY);   //
  //           不加这句边缘绘制出来有问题

  //           GdiplusRenderTarget::DrawBitmap(&g, p, 0, 0, pParam->wDest,
  //           pParam->hDest,
  //               pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc,
  //               pParam->pRegion, true, pImageAttribute);
  //       }

  //       // 圆角处理
  //       int& w = pParam->wDest;
  //       int& h = pParam->hDest;
  //       if (w > 8 && h > 8)
  //       {
  //           BITMAP bm = {0};
  //           ::GetObject(hCacheBitmap, sizeof(bm), &bm);
  //           byte* pBits = (byte*)bm.bmBits;
  //           int   nPitch = bm.bmWidthBytes;

  //           // 25%透
  //           POINT ptAlpha0[4] = { {0, 0}, {w-1, 0}, {0, h-1}, {w-1, h-1} };
  //           for (int i = 0; i < 4; i++)
  //           {
  //               byte* pAlpha = pBits + (ptAlpha0[i].y*nPitch +
  //               (ptAlpha0[i].x<<2)); byte a = ((int)*(pAlpha+3)) >> 4;
  //               *pAlpha = (*pAlpha) * a >> 8;   // r
  //               pAlpha++;
  //               *pAlpha = (*pAlpha) * a >> 8;   // g
  //               pAlpha++;
  //               *pAlpha = (*pAlpha) * a >> 8;   // b
  //               pAlpha++;
  //               *pAlpha = a;                    // a
  //           }

  //           // 50%透
  //           POINT ptAlpha1[8] = { {1, 0}, {w-2, 0}, {0, 1}, {w-1,1},  {0,
  //           h-2}, {w-1, h-2}, {1, h-1}, {w-2, h-1} }; for (int i = 0; i < 8;
  //           i++)
  //           {
  //               byte* pAlpha = pBits + (ptAlpha1[i].y*nPitch +
  //               (ptAlpha1[i].x<<2)); byte a = ((int)*(pAlpha+3)) >> 2;
  //               *pAlpha = (*pAlpha) * a >> 8;
  //               pAlpha++;
  //               *pAlpha = (*pAlpha) * a >> 8;
  //               pAlpha++;
  //               *pAlpha = (*pAlpha) * a >> 8;
  //               pAlpha++;
  //               *pAlpha = a;
  //           }

  //           // 75%
  //           POINT ptAlpha2[8] = { {2, 0}, {w-3, 0}, {0, 2}, {w-1,2},  {0,
  //           h-3}, {w-1, h-3}, {2, h-1}, {w-3, h-1} }; for (int i = 0; i < 8;
  //           i++)
  //           {
  //               byte* pAlpha = pBits + (ptAlpha2[i].y*nPitch +
  //               (ptAlpha2[i].x<<2)); byte a = ((int)*(pAlpha+3)) * 3 >> 2;
  //               *pAlpha = (*pAlpha) * a >> 8;
  //               pAlpha++;
  //               *pAlpha = (*pAlpha) * a >> 8;
  //               pAlpha++;
  //               *pAlpha = (*pAlpha) * a >> 8;
  //               pAlpha++;
  //               *pAlpha = a;
  //           }
  //       }

  //       // 提交
  //       BLENDFUNCTION bf = {AC_SRC_OVER, 0, pParam->nAlpha, AC_SRC_ALPHA};
  //       AlphaBlend(hBindDC, pParam->xDest, pParam->yDest, w, h, hDC, 0, 0, w,
  //       h, bf);

  //       ::SelectObject(hDC, hOldBitmap);
  //       Image::ReleaseCacheDC(hDC);

  //       if (pParam->prcRealDraw)
  //       {
  //           pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest,
  //           pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
  //       }
  //   }
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

// software，将子layer画到父layer上面。
void SkiaRenderTarget::Render2Target(IRenderTarget *pDst,
                                     Render2TargetParam *pParam) {
  if (!m_sksurface) {
    return;
  }

  SkiaRenderTarget *target = static_cast<SkiaRenderTarget *>(pDst);
  if (!target->m_sksurface) {
    return;
  }

  SkCanvas *target_canvas = target->m_sksurface->getCanvas();
  sk_sp<SkImage> source_image(m_sksurface->makeImageSnapshot());
  if (!source_image) {
    return;
  }

  SkSamplingOptions options;
  SkPaint paint;
  target_canvas->drawImageRect(
      source_image,
      SkRect::MakeXYWH(pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc),
      SkRect::MakeXYWH(pParam->xDst, pParam->yDst, pParam->wDst, pParam->hDst),
      options, &paint, SkCanvas::kFast_SrcRectConstraint);
}

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

void SkiaRenderTarget::Upload2Gpu(IGpuLayer *p, Rect *prcArray, int nCount) {

  SkPixmap pm;
  if (!m_sksurface || !m_sksurface->peekPixels(&pm)) {
    return;
  }

  ui::UploadGpuBitmapInfo source = {0};
  source.hasAlphaChannel = true;
  source.pFirstLineBits = (byte *)pm.addr();
  source.pitch = (int)pm.rowBytes();
  source.width = pm.width();
  source.height = pm.height();
  source.bpp = 32;

  source.prcArray = prcArray;
  source.nCount = nCount;

  p->UploadHBITMAP(source);
}

} // namespace ui