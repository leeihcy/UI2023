#include "src/graphics/async_render_target.h"
#include "include/macro/xmldefine.h"
#include "src/resource/font.h"
#include "src/thread/paint_op.h"
#include "src/thread/render_thread.h"
#include "src/resource/res_bundle.h"
#include "third_party/skia/src/include/core/SkFont.h"
#include "third_party/skia/src/include/core/SkFontMetrics.h"
#include "third_party/skia/src/include/core/SkTextBlob.h"
#include <cassert>
#include <memory>

namespace ui {

AsyncRenderTarget::AsyncRenderTarget() {
  m_rt_async = std::make_unique<SkiaRenderTarget>();
}
AsyncRenderTarget::~AsyncRenderTarget() {
  SkiaRenderTarget* ptr = m_rt_async.release();

  // 交由渲染线程进行安全释放。
  RenderThread::GetIntance().main.RemoveKey(ptr);
}

void AsyncRenderTarget::Release() {
}

void AsyncRenderTarget::addPaintOp(std::unique_ptr<PaintOp> &&paint_op) {
  paint_op->key = m_rt_async.get();

  // 仅在EndDraw时notify，便于优化合并指令
  bool notify = paint_op->type == PaintOpType::EndDraw ||
                paint_op->type == PaintOpType::BeginDraw;

  auto& thread = RenderThread::GetIntance();
  thread.main.AddPaintOp(std::move(paint_op));
  if (notify) {
    thread.main.Notify();
  }
}

bool AsyncRenderTarget::BeginDraw(const DirtyRegion& dirty_region, bool clear, float scale) {
  m_scale = scale;
  m_clip_origin_impl.SetDirtyRegion(dirty_region);
  addPaintOp(std::move(std::make_unique<BeginDrawOp>(dirty_region, clear, scale)));
  return true;
}
void AsyncRenderTarget::EndDraw() {
  addPaintOp(std::move(std::make_unique<EndDrawOp>()));
}
bool AsyncRenderTarget::Resize(unsigned int width, unsigned int height) {
  addPaintOp(std::move(std::make_unique<ResizeOp>(width, height)));
  return true;
}
void *AsyncRenderTarget::GetHandle() {
  assert(false && "NotImplement");
  return nullptr;
}

void AsyncRenderTarget::Save() {
  addPaintOp(std::move(std::make_unique<SaveOp>()));
}
void AsyncRenderTarget::Restore() {
  addPaintOp(std::move(std::make_unique<RestoreOp>()));
}
void AsyncRenderTarget::ClipRoundRect(const Rect &rect, const CornerRadius& radius) {
  addPaintOp(std::move(std::make_unique<ClipRoundRectOp>(rect, radius)));
}
void AsyncRenderTarget::ClipRect(const Rect &rect) {
  addPaintOp(std::move(std::make_unique<ClipRectOp>(rect)));
}

void AsyncRenderTarget::CreateSwapChain(bool is_hardware, IGpuCompositor* key) {
  assert(false && "Should not run by record");
  // addPaintOp(std::move(std::make_unique<CreateSwapChainOp>(is_hardware, key)));
}
bool AsyncRenderTarget::SwapChain(slot<void()> &&callback) {
  addPaintOp(std::move(std::make_unique<SwapChainOp>(std::move(callback))));
  return true;
}

void AsyncRenderTarget::DumpToImage(const char *path) {
  addPaintOp(std::move(std::make_unique<DumpToImageOp>(path)));
}

bool AsyncRenderTarget::GetFrontFrameBuffer(FrameBufferWithReadLock *fb) {
  // 在UI线程上触发的，需要同步获取缓存数据。这里带锁进行跨线程获取
  return RenderThread::GetIntance().main.GetFrontFrameBuffer(m_rt_async.get(), fb);
}

void AsyncRenderTarget::RenderOnThread(
    slot<void(IRenderTarget *)> &&callback) {
  addPaintOp(
      std::move(std::make_unique<RenderOnThreadOp>(std::move(callback))));
}

void AsyncRenderTarget::PushRelativeClipRect(const Rect &rect) {
  m_clip_origin_impl.PushRelativeClipRect(rect);
}
void AsyncRenderTarget::PopRelativeClipRect() {
  m_clip_origin_impl.PopRelativeClipRect();
}
void AsyncRenderTarget::SetOrigin(int x, int y) {
  m_clip_origin_impl.SetOrigin(x, y);
  addPaintOp(std::move(std::make_unique<SetOriginOp>(x, y)));
}
void AsyncRenderTarget::OffsetOrigin(int x, int y) {
  m_clip_origin_impl.OffsetOrigin(x, y);
  addPaintOp(std::move(std::make_unique<OffsetOriginOp>(x, y)));
}

bool AsyncRenderTarget::IsRelativeRectInClip(const Rect &rect) {
  return m_clip_origin_impl.IsRelativeRectInClip(rect);
}

void AsyncRenderTarget::Render2Target(IRenderTarget *pDst,
                                       Render2TargetParam *pParam) {
  // Render this target to another target
}

void AsyncRenderTarget::FillRect(const Rect &rect, const Color &color) {
  addPaintOp(std::move(std::make_unique<FillRectOp>(rect, color)));
}
void AsyncRenderTarget::StrokeRect(const Rect &rect, const Color &color,
                                    int width, bool dash) {
  addPaintOp(std::move(std::make_unique<StrokeRectOp>(rect, color, width, dash)));
}
void AsyncRenderTarget::FillRoundRect(const Rect &rect, const Color &color,
                                       const CornerRadius &radius) {
  addPaintOp(std::move(std::make_unique<FillRoundRectOp>(rect, color, radius)));
}
void AsyncRenderTarget::StrokeRoundRect(const Rect &rect, const Color &color,
                                         const CornerRadius &radius,
                                         int width, bool dash) {
  addPaintOp(
      std::move(std::make_unique<StrokeRoundRectOp>(rect, color, radius, width, dash)));
}

void AsyncRenderTarget::DrawBitmap(std::shared_ptr<IRenderBitmap> bitmap,
                                    DrawBitmapParam *param) {
  // TODO: 支持多线程！
  addPaintOp(std::move(std::make_unique<DrawBitmapOp>(bitmap, param)));
}

std::string elideTextWithEllipsis(const char *text, const SkRect &bounds,
                                  const SkFont &font, const SkPaint &paint,
                                  /*out*/ int &measure_width);

void AsyncRenderTarget::DrawString(const DrawTextParam &param) {
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
  // canvas->drawSimpleText(elide_text.c_str(), elide_text.length(),
  //                        SkTextEncoding::kUTF8, x, y, font, paint);

  sk_sp<SkTextBlob> text_blob =
      SkTextBlob::MakeFromString(elide_text.c_str(), font);
  // const int glyph_count = font.countText(
  //     elide_text.c_str(), elide_text.length(), SkTextEncoding::kUTF8);

  // SkTextBlobBuilder builder;
  // const auto &run_buffer = builder.allocRunPos(font, glyph_count);

  // static_assert(sizeof(*glyphs) == sizeof(*run_buffer.glyphs), "");
  // memcpy(run_buffer.glyphs, glyphs, glyph_count * sizeof(*glyphs));
  // static_assert(sizeof(*pos) == 2 * sizeof(*run_buffer.pos), "");
  // memcpy(run_buffer.pos, pos, glyph_count * sizeof(*pos));

  addPaintOp(
      std::move(std::make_unique<DrawStringOp>(text_blob, param.color, x, y)));
}

void AsyncRenderTarget::drawString2(void *text_blob, const Color &color,
                                      float x, float y) {
  assert(false && "Not Implement");
}

} // namespace ui