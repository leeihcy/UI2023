#include "skia_render.h"
#include "skia_bitmap.h"

#include "SkStream.h"
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "include/core/SkBitmap.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkScalar.h"
#include "include/gpu/GrTypes.h"
#include "include/inc.h"
#include "include/interface/renderlibrary.h"
#include "include/macro/xmldefine.h"
#include "include/util/log.h"
#include "include/util/rect.h"

#include "gpu/include/api.h"

#include "src/application/config/config.h"
#include "src/graphics/font/font.h"
#include "src/layer/windowrender.h"
#include "src/resource/res_bundle.h"
#include "third_party/skia/src/include/core/SkBitmap.h"
#include "third_party/skia/src/include/core/SkCanvas.h"
#include "third_party/skia/src/include/core/SkFont.h"
#include "third_party/skia/src/include/core/SkFontMetrics.h"
#include "third_party/skia/src/include/core/SkPaint.h"
#include "third_party/skia/src/include/core/SkPath.h"
#include "third_party/skia/src/include/core/SkRRect.h"
#include "third_party/skia/src/include/core/SkRect.h"
#include "third_party/skia/src/include/core/SkTextBlob.h"
#include "third_party/skia/src/include/core/SkPathEffect.h"
#include "third_party/skia/src/include/effects/SkDashPathEffect.h"

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

SkiaRenderTarget::SkiaRenderTarget() {
  
}

SkiaRenderTarget::~SkiaRenderTarget() {
}

void SkiaRenderTarget::Release() { delete this; }

void SkiaRenderTarget::update_clip_rgn() {
  // canvas->clipRect 不支持replace操作，导致clip区域越来越小，无法还原
  // 只能通过 save/restore 来还原clip区域。

  SkCanvas *canvas = m_sksurface->getCanvas();

  SkPath clip_path;
  int count = m_clip_origin_impl.m_dirty_region.Count();
  for (int i = 0; i < count; i++) {
    Rect *prc = m_clip_origin_impl.m_dirty_region[i];

    SkRect skrc;
    toSkRect(*prc, &skrc);
    clip_path.addRect(skrc);
  }

  if (!m_clip_origin_impl.m_stackClipRect.empty()) {
    auto iter = m_clip_origin_impl.m_stackClipRect.begin();

    Rect rcIntersect = *iter;
    iter++;

    for (; iter != m_clip_origin_impl.m_stackClipRect.end(); ++iter) {
      rcIntersect.Intersect(*iter, &rcIntersect);
    }

    SkRect skrc;
    toSkRect(rcIntersect, &skrc);
    clip_path.addRect(skrc);
  }

  if (!clip_path.isEmpty()) {
    canvas->clipPath(clip_path);
  }
}

void SkiaRenderTarget::SetDirtyRegion(const DirtyRegion &dirty_region) {
  if (Config::GetInstance().debug.log_paint) {
    UI_LOG_DEBUG("[SkiaRenderTarget] SetDirtyRegion count = %d",
                 dirty_region.Count());
    for (uint i = 0; i < dirty_region.Count(); i++) {
      const Rect &rc = dirty_region.RectPtr2()[i];
      UI_LOG_DEBUG("        %d,%d, (%d,%d)", rc.left, rc.top, rc.Width(),
                   rc.Height());
    }
  }

  m_clip_origin_impl.SetDirtyRegion(dirty_region);
  update_clip_rgn();

  if (m_enable_software_backend) {
    frames_sync_dirty();
  }
}
const DirtyRegion &SkiaRenderTarget::GetDirtyRegion() {
  return m_clip_origin_impl.GetDirtyRegion();
}

void SkiaRenderTarget::PushRelativeClipRect(const Rect &rect) {
  m_clip_origin_impl.PushRelativeClipRect(rect);
  // update_clip_rgn();

  // Rect rc = rect;
  // rc.Offset(m_clip_origin_impl.m_ptOffset.x, m_clip_origin_impl.m_ptOffset.y);

  Save();
  ClipRect(rect);
}

void SkiaRenderTarget::PopRelativeClipRect() {
  m_clip_origin_impl.PopRelativeClipRect();
  // update_clip_rgn();

  Restore();
}

bool SkiaRenderTarget::IsRelativeRectInClip(const Rect &rect) {
  return m_clip_origin_impl.IsRelativeRectInClip(rect);
}

void SkiaRenderTarget::SetOrigin(int x, int y) {
  int old_offset_x = 0;
  int old_offset_y = 0;
  m_clip_origin_impl.GetOrigin(old_offset_x, old_offset_y);

  if (old_offset_x == x && old_offset_y == y)
    return;

  int offsetx = x - old_offset_x;
  int offsety = y - old_offset_y;

  m_clip_origin_impl.SetOrigin(x, y);

  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->translate((SkScalar)offsetx, (SkScalar)offsety);
}
void SkiaRenderTarget::OffsetOrigin(int x, int y) {
  m_clip_origin_impl.OffsetOrigin(x, y);

  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->translate((SkScalar)x, (SkScalar)y);
}

// bool SkiaRenderTarget::CreateRenderBuffer(IRenderTarget *pSrcRT) {
//   return true;
// }

// 为了加快鼠标拖拽窗口的Resize效率
bool SkiaRenderTarget::Resize(unsigned int width, unsigned int height) {
  m_resized_flags = true;
  m_width = width;
  m_height = height;

  resize_gpu_layer(width, height);

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

    fix_width = std::max(m_sksurface->width(), fix_width);
    fix_height = std::max(m_sksurface->height(), fix_height);
  }

  UI_LOG_INFO("[SkiaRenderTarget] Resize:%d,%d => %d,%d(0x%x, 0x%x)",
              width, height, fix_width, fix_height, fix_width, fix_height);

  m_sksurface = create_surface(fix_width, fix_height);

  // test，还没刷新界面，就直接commit的话，会出现全黄色。
  // #if defined(DEBUG)
  //   SkCanvas *canvas = m_sksurface->getCanvas();
  //   canvas->clear(SK_ColorYELLOW);
  // #endif
  // m_sksurface = SkSurface::MakeRasterN32Premul(width, height);
  // SkCanvas *canvas = m_sksurface->getCanvas();

  return true;
}

sk_sp<SkSurface> SkiaRenderTarget::create_surface(unsigned int width,
                                                  unsigned int height) {
  SkImageInfo info = SkImageInfo::Make(width, height, kBGRA_8888_SkColorType,
                                       kPremul_SkAlphaType);
  SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);

  return SkSurface::MakeRaster(info, &surfaceProps);
}

void SkiaRenderTarget::resize_gpu_layer(unsigned int width,
                                        unsigned int height) {
  if (m_gpu_texture) {
    m_gpu_texture->Resize(width * m_scale, height * m_scale);
  }
}

void *SkiaRenderTarget::GetHandle() {
  SkCanvas *canvas = m_sksurface->getCanvas();
  return canvas;
}

void SkiaRenderTarget::Save() {
  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->save();
}
void SkiaRenderTarget::Restore() {
  SkCanvas *canvas = m_sksurface->getCanvas();
  canvas->restore();
}

void SkiaRenderTarget::ClipRoundRect(const Rect &rect, const CornerRadius& radius) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkScalar ul = (SkScalar)radius.top_left;
  SkScalar ur = (SkScalar)radius.top_right;
  SkScalar ll = (SkScalar)radius.bottom_left;
  SkScalar lr = (SkScalar)radius.bottom_right;

  SkRRect rr;
  SkVector radii[4] = {
      {ul, ul},
      {ur, ur},
      {lr, lr},
      {ll, ll},
  };
  rr.setRectRadii(
      SkRect::MakeXYWH(rect.left, rect.top, rect.Width(), rect.Height()),
      radii);

  SkPath path;
  path.addRRect(rr);

  canvas->clipPath(path, SkClipOp::kIntersect, true);
}
void SkiaRenderTarget::ClipRect(const Rect &rect) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkPath path;
  path.addRect(
      SkRect::MakeXYWH(rect.left, rect.top, rect.Width(), rect.Height()));
  canvas->clipPath(path, SkClipOp::kIntersect, true);
}

// void SkiaRenderTarget::GetRenderBufferData(ImageData *pData) {
//   // if (!m_pRenderBuffer)
//   // 	return;
//   // m_pRenderBuffer->GetImageData(pData);
// }

bool SkiaRenderTarget::BeginDraw(float scale) {
  if (Config::GetInstance().debug.log_paint) {
    UI_LOG_DEBUG("[SkiaRenderTarget] BeginDraw back surface=0x%08x",
                 m_sksurface.get());
  }

  if (!m_sksurface) {
    return false;
  }

  if (m_enable_software_backend) {
    frames_sync_size();
  }

  SkCanvas *canvas = m_sksurface->getCanvas();
  if (!canvas) {
    return false;
  }
  m_save_count = canvas->save();

  canvas->scale(scale, scale);
  m_scale = scale;

  return true;
}

void SkiaRenderTarget::EndDraw() {
  m_resized_flags = false;
  m_last_dirty_region = m_clip_origin_impl.GetDirtyRegion();

  if (!m_sksurface) {
    return;
  }

  SkCanvas *canvas = m_sksurface->getCanvas();
  if (!canvas) {
    return;
  }
  canvas->restoreToCount(m_save_count);

  if (m_enable_hardware_backend) {
    upload_2_gpu();
  }
  
  if (Config::GetInstance().debug.log_paint) {
    UI_LOG_DEBUG("[SkiaRenderTarget] EndDraw");
  }
}

// 将front/back大小进行同步
void SkiaRenderTarget::frames_sync_size() {
  if (!m_sksurface || !main.m_sksurface_front) {
    return;
  }

  unsigned int front_width = main.m_sksurface_front->width();
  unsigned int front_height = main.m_sksurface_front->height();

  bool full_dirty = false;
  if (!m_sksurface) {
    m_sksurface = create_surface(front_width, front_height);
    full_dirty = true;
  } else if (!m_resized_flags) {
    // 只在本次外部没用调用Resize时，再同步上一轮的大小。
    // 由于render target size的冗余特性，
    // 出现front/back的最终大小不一致是可能的，
    // 只要满足大于surface大小即可。
    if (m_sksurface->width() != front_width ||
        m_sksurface->height() != front_height) {
      m_sksurface = create_surface(front_width, front_height);
      full_dirty = true;
    }
  }
  if (full_dirty) {
    m_last_dirty_region.Destroy();
    m_last_dirty_region.AddRect(ui::Rect::MakeXYWH(0, 0, m_width, m_height));
  }
}


void SkiaRenderTarget::Clear(const Rect &rect) {
  if (!m_sksurface) {
    UI_LOG_WARN(L"no sksurface");
    return;
  }
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkRect skrect;
  skrect.fLeft = (SkScalar)rect.left;
  skrect.fTop = (SkScalar)rect.top;
  skrect.fRight = (SkScalar)rect.right;
  skrect.fBottom = (SkScalar)rect.bottom;

  SkPaint paint;
  paint.setColor(SK_ColorTRANSPARENT);
  paint.setBlendMode(SkBlendMode::kSrc);
  canvas->drawRect(skrect, paint);
}

std::string elideTextWithEllipsis(const char *text, const SkRect &bounds,
                                  const SkFont &font, const SkPaint &paint,
                                  /*out*/ int &measure_width) {
  if (!text) {
    return std::string();
  }
  float textWidth = font.measureText(text, strlen(text), SkTextEncoding::kUTF8);
  if (textWidth <= bounds.width()) {
    measure_width = textWidth;
    return std::string(text);
  }

  const char *ellipsis = "...";
  const size_t ellipsisLen = strlen(ellipsis);
  float ellipsisWidth =
      font.measureText(ellipsis, ellipsisLen, SkTextEncoding::kUTF8);

  float availableWidth = bounds.width() - ellipsisWidth;
  if (availableWidth <= 0) {
    measure_width = availableWidth;

    // 如果连省略号都显示不下，只显示省略号
    return std::string(ellipsis);
  }

  // 查找适合的文本截断位置（按UTF-8字符截断）
  size_t byteLength = strlen(text);
  float width = 0;
  while (byteLength > 0) {
    width = font.measureText(text, byteLength, SkTextEncoding::kUTF8);
    if (width <= availableWidth) {
      break;
    }
    // 向前找到上一个UTF-8字符的起始位置
    do {
      --byteLength;
    } while (byteLength > 0 && ((text[byteLength] & 0xC0) == 0x80));
  }

  measure_width = ellipsisWidth + width;
  return std::string(text, byteLength) + ellipsis;
}

#if 0
static void drawMultilineText(SkCanvas* canvas, const SkString& text, const SkRect& bounds, const SkPaint& paint) {
    std::vector<SkString> lines;
    SkPaint::FontMetrics metrics;
    paint.getFontMetrics(&metrics);
    
    float lineHeight = metrics.fDescent - metrics.fAscent + metrics.fLeading;
    float y = bounds.top() - metrics.fAscent;
    
    // 简单的换行算法
    const char* start = text.c_str();
    const char* end = start + text.size();
    const char* lineEnd = start;
    const char* space = nullptr;
    
    while (start < end) {
        // 查找下一个空格或行尾
        space = nullptr;
        lineEnd = start;
        
        while (lineEnd < end) {
            float lineWidth = paint.measureText(start, lineEnd - start + 1);
            if (lineWidth > bounds.width()) {
                if (space) {
                    // 在最后一个空格处换行
                    lineEnd = space;
                }
                break;
            }
            
            if (*lineEnd == ' ' || *lineEnd == '\n') {
                space = lineEnd;
                if (*lineEnd == '\n') {
                    break;
                }
            }
            
            lineEnd++;
        }
        
        // 添加行
        lines.emplace_back(start, lineEnd - start);
        
        // 移动到下一行
        start = lineEnd;
        while (start < end && (*start == ' ' || *start == '\n')) {
            start++;
        }
        
        y += lineHeight;
        if (y + metrics.fDescent > bounds.bottom()) {
            break; // 超出边界停止绘制
        }
    }
    
    // 绘制所有行
    y = bounds.top() - metrics.fAscent;
    for (const auto& line : lines) {
        if (y + metrics.fDescent > bounds.bottom()) {
            break;
        }
        
        canvas->drawString(line, bounds.left(), y, paint);
        y += lineHeight;
    }
}
#endif

// Text baseline offset types.
// Figure of font metrics:
//   +--------+--------+------------------------+-------------+
//   |        |        | internal leading       | SUPERSCRIPT |
//   |        |        +------------+-----------|             |
//   |        | ascent |            | SUPERIOR  |-------------+
//   | height |        | cap height |-----------|
//   |        |        |            | INFERIOR  |-------------+
//   |        |--------+------------+-----------|             |
//   |        | descent                         | SUBSCRIPT   |
//   +--------+---------------------------------+-------------+
//
void SkiaRenderTarget::DrawString(const DrawTextParam &param) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkFont &font = param.resource->GetImpl()->GetFontRes().GetSkiaFont(param.font_desc);

  SkPaint paint;
  paint.setColor(param.color.value);

  SkRect sk_rect = SkRect::MakeXYWH(
      (SkScalar)param.bound.left, (SkScalar)param.bound.top,
      (SkScalar)param.bound.Width(), (SkScalar)param.bound.Height());

  int measure_width = 0;
  std::string elide_text =
      elideTextWithEllipsis(param.text, sk_rect, font, paint, measure_width);

  // drawSimpleString纵坐标参数表示文本基线的位置，而不是文本的顶部或底部
  SkFontMetrics metrics;
  font.getMetrics(&metrics);
  float totalHeight = metrics.fDescent - metrics.fAscent + metrics.fLeading;

  SkScalar x = param.bound.left;
  SkScalar y = param.bound.top;

  // 基线位置 += 字体的上升高度（ascent是负值，需取反）
  y = y - metrics.fAscent;

  if (!param.multiline) {
    if (param.align & AlignCenter) {
      x += (param.bound.width() - measure_width) / 2;
    } else if (param.align & AlignRight) {
      x += param.bound.width() - measure_width;
    }
    if (param.align & AlignVCenter) {
      y += (param.bound.height() - totalHeight) / 2;
    } else if (param.align & AlignBottom) {
      y += param.bound.height() - totalHeight;
    }
  }

  canvas->drawSimpleText(elide_text.c_str(), elide_text.length(),
                         SkTextEncoding::kUTF8, x, y, font, paint);
  return;
}

void SkiaRenderTarget::drawString2(void *text_blob, const Color &color,
                                    float x, float y) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkPaint paint;
  paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
  canvas->drawTextBlob((SkTextBlob *)text_blob, x, y, paint);
}

// void SkiaRenderTarget::FillRgn(/*HRGN*/ llong hRgn, ui::Color *pColor) {
//   UIASSERT(false);
// }

void SkiaRenderTarget::FillRect(const Rect &rect, const Color &color) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkRect skrect;
  skrect.fLeft = (SkScalar)rect.left;
  skrect.fTop = (SkScalar)rect.top;
  skrect.fRight = (SkScalar)rect.right;
  skrect.fBottom = (SkScalar)rect.bottom;

  SkPaint paint;
  paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
  canvas->drawRect(skrect, paint);
}

void SkiaRenderTarget::StrokeRect(const Rect &rect, const Color &color, int width, bool dash) {
  if (width <= 0) {
    return;
  }
  SkCanvas *canvas = m_sksurface->getCanvas();

  // 默认边框是居中绘制的，这里调整为靠内侧绘制
  SkScalar offset = width/2.0f;

  SkRect skrect;
  skrect.fLeft = (SkScalar)rect.left + offset;
  skrect.fTop = (SkScalar)rect.top + offset;
  skrect.fRight = (SkScalar)rect.right - offset;
  skrect.fBottom = (SkScalar)rect.bottom - offset;

  SkPaint paint;
  paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
  paint.setStroke(true);
  paint.setStrokeWidth(width);

  if (dash) {
      // 1. 定义虚线模式：实线x像素，间隔y像素
    const SkScalar intervals[] = {2.0f, 2.0f};
    const int count = 2; // intervals数组的元素个数
    const SkScalar phase = 0; // 相位，从模式起点开始

    // 2. 创建虚线路径效果
    sk_sp<SkPathEffect> dashEffect = SkDashPathEffect::Make(intervals, count, phase);

    // 3. 为画笔设置路径效果
    paint.setPathEffect(dashEffect);
  }
  
  canvas->drawRect(skrect, paint);
}

void SkiaRenderTarget::FillRoundRect(const Rect &rect, const Color &color,
                     const CornerRadius &radius) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  SkRect skrect;
  skrect.fLeft = (SkScalar)rect.left;
  skrect.fTop = (SkScalar)rect.top;
  skrect.fRight = (SkScalar)rect.right;
  skrect.fBottom = (SkScalar)rect.bottom;

  SkPaint paint;
  paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
  paint.setAntiAlias(true);
  
  SkVector radii[4] = {
      {(SkScalar)radius.top_left, (SkScalar)radius.top_left},
      {(SkScalar)radius.top_right, (SkScalar)radius.top_right},
      {(SkScalar)radius.bottom_right, (SkScalar)radius.bottom_right},
      {(SkScalar)radius.bottom_left, (SkScalar)radius.bottom_left},
  };
  SkRRect rrect;
  rrect.setRectRadii(skrect, radii);
  canvas->drawRRect(rrect, paint);
}

void SkiaRenderTarget::StrokeRoundRect(const Rect &rect, const Color &color,
                     const CornerRadius &radius, int width, bool dash) {
  SkCanvas *canvas = m_sksurface->getCanvas();

  // 默认边框是居中绘制的，这里调整为靠内侧绘制
  SkScalar offset = width/2.0f;

  SkRect skrect;
  skrect.fLeft = (SkScalar)rect.left + offset;
  skrect.fTop = (SkScalar)rect.top + offset;
  skrect.fRight = (SkScalar)rect.right - offset;
  skrect.fBottom = (SkScalar)rect.bottom - offset;

  SkPaint paint;
  paint.setStroke(true);
  paint.setStrokeWidth(width);
  paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
  paint.setAntiAlias(true);

  if (dash) {
      // 1. 定义虚线模式：实线x像素，间隔y像素
    const SkScalar intervals[] = {2.0f, 2.0f};
    const int count = 2; // intervals数组的元素个数
    const SkScalar phase = 0; // 相位，从模式起点开始

    // 2. 创建虚线路径效果
    sk_sp<SkPathEffect> dashEffect = SkDashPathEffect::Make(intervals, count, phase);

    // 3. 为画笔设置路径效果
    paint.setPathEffect(dashEffect);
  }

  SkVector radii[4] = {
      {(SkScalar)radius.top_left, (SkScalar)radius.top_left},
      {(SkScalar)radius.top_right, (SkScalar)radius.top_right},
      {(SkScalar)radius.bottom_right, (SkScalar)radius.bottom_right},
      {(SkScalar)radius.bottom_left, (SkScalar)radius.bottom_left},
  };
  SkRRect rrect;
  rrect.setRectRadii(skrect, radii);
  canvas->drawRRect(rrect, paint);
}
// void SkiaRenderTarget::TileRect(Rect *lprc, IRenderBitmap *pRenderBitmap) {
//   UIASSERT(false);
// }

// void SkiaRenderTarget::Rectangle(Rect *lprc, ui::Color *pColBorder,
//                                  ui::Color *pColBack, int nBorder,
//                                  bool bNullBack) {
//   UIASSERT(false);
// }

// void SkiaRenderTarget::DrawFocusRect(Rect *lprc) { UIASSERT(false); }
// void SkiaRenderTarget::DrawLine(int x1, int y1, int x2, int y2,
//                                 IRenderPen *pPen) {
//   UIASSERT(false);
// }
// void SkiaRenderTarget::DrawPolyline(Point *lppt, int nCount, IRenderPen
// *pPen) {
//   UIASSERT(false);
// }
// void SkiaRenderTarget::GradientFillH(Rect *lprc, Color colFrom, Color colTo)
// {
//   //	util::GradientFillH(GetHDC(), lprc, colFrom, colTo );
// }
// void SkiaRenderTarget::GradientFillV(Rect *lprc, Color colFrom, Color colTo)
// {
//   //	util::GradientFillV(GetHDC(), lprc, colFrom, colTo );
// }
//
// void SkiaRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest,
//                               IRenderTarget *pSrcHDC, int xSrc, int ySrc,
//                               unsigned int dwRop) {
//   UIASSERT(false);
// }
//
// void SkiaRenderTarget::ImageList_Draw(IRenderBitmap *hBitmap, int x, int y,
//                                       int col, int row, int cx, int cy) {
//   UIASSERT(false);
// }

void SkiaRenderTarget::DrawBitmap(std::shared_ptr<IRenderBitmap> pRenderBitmap,
                                  DRAWBITMAPPARAM *pParam) {
  if (NULL == pRenderBitmap || NULL == pParam)
    return;

  if (pRenderBitmap->GetGraphicsRenderLibraryType() !=
      GRAPHICS_RENDER_LIBRARY_TYPE_SKIA) {
    return;
  }

  SkiaRenderBitmap *skia_bitmap =
      static_cast<SkiaRenderBitmap *>(pRenderBitmap.get());
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
    // if (pParam->scale_factor == 0 || pParam->scale_factor == 1.0) {
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
        SkRect::MakeXYWH((SkScalar)pParam->xSrc, (SkScalar)pParam->ySrc,
                         (SkScalar)src_width, (SkScalar)src_height),
        SkRect::MakeXYWH((SkScalar)pParam->xDest, (SkScalar)pParam->yDest,
                         (SkScalar)dest_width, (SkScalar)dest_height),
        options, &paint, SkCanvas::kFast_SrcRectConstraint);

    // if (pParam->prcRealDraw) {
    //   pParam->prcRealDraw->Set(pParam->xDest, pParam->yDest,
    //                            pParam->xDest + dest_width,
    //                            pParam->yDest + dest_height);
    // }
  } else if (pParam->nFlag & DRAW_BITMAP_STRETCH) {
    SkSamplingOptions options;
    SkPaint paint;
    canvas->drawImageRect(
        skia_bitmap->m_bitmap.asImage(),
        SkRect::MakeXYWH((SkScalar)pParam->xSrc, (SkScalar)pParam->ySrc,
                         (SkScalar)pParam->wSrc, (SkScalar)pParam->hSrc),
        SkRect::MakeXYWH((SkScalar)pParam->xDest, (SkScalar)pParam->yDest,
                         (SkScalar)pParam->wDest, (SkScalar)pParam->hDest),
        options, &paint, SkCanvas::kFast_SrcRectConstraint);

    // if (pParam->prcRealDraw) {
    //   pParam->prcRealDraw->Set(pParam->xDest, pParam->yDest,
    //                            pParam->xDest + pParam->wDest,
    //                            pParam->yDest + pParam->hDest);
    // }
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

// IRenderPen *SkiaRenderTarget::CreateSolidPen(int nWidth, Color *pColor) {
//   // IRenderPen* p = nullptr;
//   // GdiPen::CreateInstance(&p);

//   // if (p)
//   // {
//   // 	p->CreateSolidPen(nWidth, pColor);
//   // }
//   // return p;
//   UIASSERT(false);
//   return nullptr;
// }
// IRenderPen *SkiaRenderTarget::CreateDotPen(int nWidth, Color *pColor) {
//   // IRenderPen* p = nullptr;
//   // GdiPen::CreateInstance(&p);

//   // if (p)
//   // {
//   // 	p->CreateDotPen(nWidth, pColor);
//   // }
//   // return p;
//   UIASSERT(false);
//   return nullptr;
// }
// IRenderBrush *SkiaRenderTarget::CreateSolidBrush(Color *pColor) {
//   // IRenderBrush* p = nullptr;
//   // GdiBrush::CreateInstance(&p);

//   // if (p)
//   // {
//   // 	p->CreateSolidBrush(pColor);
//   // }
//   // return p;
//   UIASSERT(false);
//   return nullptr;
// }

// software，将子layer画到父layer上面。
void SkiaRenderTarget::Render2Target(IRenderTarget *pDst,
                                     Render2TargetParam *param) {

  if (!m_sksurface) {
    return;
  }

  SkiaRenderTarget *target = static_cast<SkiaRenderTarget *>(pDst);
  if (!target->m_sksurface) {
    return;
  }

  // 两个canvas都被scale了，需要将将源canvas等比例提交到target上，而不被再次放大。
  render_to_surface(m_sksurface.get(), target->m_sksurface.get(), param, m_scale);
}

void SkiaRenderTarget::render_to_surface(SkSurface *source, SkSurface *target,
                                         Render2TargetParam *param, float scale) {
  SkCanvas *target_canvas = target->getCanvas();
  SkCanvas *source_canvas = source->getCanvas();

  sk_sp<SkImage> source_image(source->makeImageSnapshot());
  if (!source_image) {
    return;
  }

  SkSamplingOptions options;
  SkPaint paint;

  {
    target_canvas->drawImageRect(
        source_image,
        SkRect::MakeXYWH(
            (SkScalar)param->xSrc * scale, (SkScalar)param->ySrc * scale,
            (SkScalar)param->wSrc * scale, (SkScalar)param->hSrc * scale),
        SkRect::MakeXYWH((SkScalar)param->xDst, (SkScalar)param->yDst,
                         (SkScalar)param->wDst, (SkScalar)param->hDst),
        options, &paint, SkCanvas::kFast_SrcRectConstraint);
  }
}

void SkiaRenderTarget::DumpToImage(const char *path) {
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

void SkiaRenderTarget::Upload2Gpu(Rect *prcArray, int nCount,
                                  float scale) {
  if (!m_gpu_texture) {
    assert(false);
    return;
  }
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

  std::vector<Rect> rects;
  rects.reserve(nCount);
  for (int i = 0; i < nCount; ++i) {
    Rect rc = prcArray[i];
    rc.Scale(scale);
    rects.push_back(rc);
  }
  source.prcArray = rects.data();
  source.nCount = nCount;

  m_gpu_texture->UploadBitmap(source);

  if (Config::GetInstance().debug.log_gpu) {
    for (int i = 0; i < nCount; ++i) {
      Rect *prc = &prcArray[i];
      UI_LOG_DEBUG(
          "Upload2Gpu: rt=0x%08x source=(%d, %d), rect[%d] = (%d, %d, %d, %d)",
          this, source.width, source.height, i, prc->left, prc->top, prc->right,
          prc->bottom);
    }
  }
}

bool SkiaRenderTarget::GetFrontFrameBuffer(FrameBufferWithReadLock *fb) {
  if (!fb) {
    return false;
  }
  if (m_enable_hardware_backend) {
    fb->gpu_layer = m_gpu_texture;
    return true;
  }

  SkPixmap pm;
  SkSurface *surface = m_sksurface.get();
  if (m_enable_software_backend) {
    std::shared_lock lock(main.m_mutex);
    fb->read_lock = std::move(lock);

    if (!main.m_sksurface_front) {
      return false;
    }
    surface = main.m_sksurface_front.get();
  }
  if (!surface) {
    return false;
  }

  if (!surface->peekPixels(&pm)) {
    return false;
  }

  fb->width = pm.width();
  fb->height = pm.height();
  fb->data = pm.addr();
  fb->rowbytes = (int)pm.rowBytes();

  //   if (Config::GetInstance().debug.dump_render_target) {
  //     static int i = 0;
  //     char path[64];
  // #if defined(OS_WIN)
  //     sprintf(path, "D:\\images\\%d_%p.png", i++, surface.front.get());
  // #else
  //     sprintf(path, "/tmp/images/%d_%p.png", i++, surface.front.get());
  // #endif
  //     surface.front->DumpToImage(path);
  //   }

  if (Config::GetInstance().debug.log_paint) {
    UI_LOG_DEBUG("[SkiaRenderTarget] GetFrontFrameBuffer front sufrace = 0x%08x", surface);
  }
  return true;
}

void SkiaRenderTarget::RenderOnThread(slot<void(IRenderTarget *)> &&callback) {
  callback.emit(static_cast<IRenderTarget *>(this));
}

void SkiaRenderTarget::upload_2_gpu() {
  if (!m_sksurface || !m_gpu_texture) {
    return;
  }
  int width = m_sksurface->width();
  int height = m_sksurface->height();

  Rect rc = {0, 0, width, height};
  Upload2Gpu(&rc, 1, m_scale);
}

void SkiaRenderTarget::CreateSwapChain(bool is_hardware, IGpuCompositor* compositor) {
  if (is_hardware) {
    if (!m_gpu_texture) {
      m_gpu_texture = compositor->CreateLayerTexture();
      if (m_gpu_texture && m_sksurface) {
        m_gpu_texture->Resize(m_sksurface->width(), m_sksurface->height());
      }
    }
    m_enable_hardware_backend = true;
  } else {
    m_enable_software_backend = true;
  }
}

bool SkiaRenderTarget::SwapChain(slot<void()> &&callback) {
  if (m_enable_hardware_backend) {
    return false;
  }
  std::unique_lock lock(main.m_mutex);

  if (!m_enable_software_backend) {
    return false;
  }
  if (!m_sksurface) {
    return false;
  }

  // 将front surface推迟到swapchain的时候再创建，有两个好处：
  // 1. 避免主线程提前获取到一个还没有数据的空白front surface去做commit操作
  // 2. 此时back surface已就绪，创建完front surface后可直接进行数据同步，让二者保持同步。
  if (!main.m_sksurface_front) {
    int width = m_sksurface->width();
    int height = m_sksurface->height();
    SkImageInfo info = SkImageInfo::Make(width, height, kBGRA_8888_SkColorType,
                                         kPremul_SkAlphaType);
    SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);
    main.m_sksurface_front = SkSurface::MakeRaster(info, &surfaceProps);

    // 创建front surface后，直接复制back surface的数据。让两者同步。
    Render2TargetParam param = {0};
    param.wDst = param.wSrc = width;
    param.hDst = param.hSrc = height;
    render_to_surface(m_sksurface.get(), main.m_sksurface_front.get(), &param, 1);

    if (Config::GetInstance().debug.log_paint) {
      UI_LOG_DEBUG("[SkiaRenderTarget] Create front surface 0x%08x",
                   main.m_sksurface_front.get());
    }
  }

  // 交换指针
  std::swap(m_sksurface, main.m_sksurface_front);
  callback.emit();

  if (Config::GetInstance().debug.log_paint) {
    UI_LOG_DEBUG("[SkiaRenderTarget] SwapChain front surface=>0x%08x, back surface=>0x%08x",
                 main.m_sksurface_front.get(), 
                m_sksurface.get());
  }
  return true;
}

// 将front/back数据内容进行同步，避免使用完back后，再使用front时，丢了一帧内容的问题。
void SkiaRenderTarget::frames_sync_dirty() {
  // read lock
  std::shared_lock lock(main.m_mutex);

  if (!main.m_sksurface_front || !m_sksurface) {
    return;
  }

  // 优化：如果本次的脏区域范围大于等于上一帧的变动范围，则不需要做帧同步
  const DirtyRegion &dirty_region_current_frame =
      m_clip_origin_impl.GetDirtyRegion();
  if (dirty_region_current_frame.Contains(m_last_dirty_region)) {
    return;
  }

  // TODO: 继续优化，不仅仅是Contains，更应该是Sub
  for (unsigned int i = 0; i < m_last_dirty_region.Count(); i++) {
    Rect *rect = m_last_dirty_region[i];
    Render2TargetParam param = {0};
    param.xSrc = param.xDst = rect->left;
    param.ySrc = param.yDst = rect->top;
    param.wSrc = param.wDst = rect->Width();
    param.hSrc = param.hDst = rect->Height();
    render_to_surface(main.m_sksurface_front.get(), m_sksurface.get(), &param, 1);
    // printf("frames sync dirty: %d,%d,  %d,%d\n", rect->left, rect->top,
    // rect->Width(), rect->Height());
  }
}

} // namespace ui