#ifndef _UI_SRK_SRC_THREAD_PAINT_OP_H_
#define _UI_SRK_SRC_THREAD_PAINT_OP_H_
#include "include/util/color.h"
#include "include/util/rect.h"
#include "src/graphics/record/record_render_target.h"
#include <memory>

#include "include/util/rect_region.h"
#include "src/layer/layer_sync_op.h"
#include "third_party/skia/src/include/core/SkRefCnt.h"
#include "third_party/skia/src/include/core/SkTextBlob.h"

namespace ui {
struct IRenderTarget;

enum class PaintOpType : unsigned char {
  // Render Op
  RenderOpStart,

  BeginDraw,
  EndDraw,
  Resize,
  Save,
  Restore,
  SetOrigin,
  OffsetOrigin,
  ClipRoundRect,
  ClipRect,
  FillRect,
  StrokeRect,
  FillRoundRect,
  StrokeRoundRect,
  DrawString,
  DrawBitmap,
  // CreateSwapChain,
  SwapChain,
  DumpToImage,
  // Upload2Gpu,
  RenderOnThread,
  
  RenderOpEnd,
  
  // --------------------------

  // post command for thread
  PostCommandStart,
  AsyncTask,
  RemoveKey,
  SyncLayerProperties,
  PostCommandEnd,
};

struct PaintOp {
  PaintOp(PaintOpType t) : type(t) {}
  virtual ~PaintOp() { }

  void processOnRenderThread(IRenderTarget* rt);

  IRenderTarget *key = nullptr;
  PaintOpType type;
};

struct RenderCommand : public PaintOp {
  RenderCommand(PaintOpType t) : PaintOp(t) {}
};

struct BeginDrawOp : public PaintOp {
public:
  BeginDrawOp(const DirtyRegion &_dirty, bool _clear, float _scale)
      : PaintOp(PaintOpType::BeginDraw), dirty_region(_dirty), clear(_clear),
        scale(_scale) {}

  DirtyRegion dirty_region;
  bool clear = false;
  float scale = 1;
};

struct EndDrawOp : public PaintOp {
  EndDrawOp() : PaintOp(PaintOpType::EndDraw) {}
};

struct ResizeOp : public PaintOp {
  ResizeOp(unsigned int _width, unsigned int _height)
      : PaintOp(PaintOpType::Resize), width(_width), height(_height) {}

  unsigned int width;
  unsigned int height;
};

struct SaveOp : public PaintOp {
  SaveOp()
      : PaintOp(PaintOpType::Save){}
};
struct RestoreOp : public PaintOp {
  RestoreOp()
      : PaintOp(PaintOpType::Restore){}
};

struct ClipRoundRectOp : public PaintOp {
  ClipRoundRectOp(const Rect& _rect, const CornerRadius& _radius)
      : PaintOp(PaintOpType::ClipRoundRect), rect(_rect), radius(_radius){}
  Rect rect;
  CornerRadius radius;
};
struct ClipRectOp : public PaintOp {
  ClipRectOp(const Rect& _rect)
      : PaintOp(PaintOpType::ClipRect), rect(_rect){}
  Rect rect;
};
struct SetOriginOp : public PaintOp {
  SetOriginOp(int _x, int _y)
      : PaintOp(PaintOpType::SetOrigin), x(_x), y(_y){}
  int x;
  int y;
};
struct OffsetOriginOp : public PaintOp {
  OffsetOriginOp(int _x, int _y)
      : PaintOp(PaintOpType::OffsetOrigin), x(_x), y(_y){}
  int x;
  int y;
};

struct FillRectOp : public PaintOp {
  FillRectOp(const Rect &_rect, const Color &_color)
      : PaintOp(PaintOpType::FillRect), rect(_rect), color(_color) {}
  Rect rect;
  Color color;
};

struct StrokeRectOp : public PaintOp {
  StrokeRectOp(const Rect &_rect, const Color &_color, int _width, bool _dash)
      : PaintOp(PaintOpType::StrokeRect), rect(_rect), color(_color),
        width(_width), dash(_dash) {}
  Rect rect;
  Color color;
  int width;
  bool dash;
};

struct FillRoundRectOp : public PaintOp {
  FillRoundRectOp(const Rect &_rect, const Color &_color,
                  const CornerRadius &_radius)
      : PaintOp(PaintOpType::FillRoundRect), rect(_rect), color(_color),
        radius(_radius) {}
  Rect rect;
  Color color;
  CornerRadius radius;
};

struct StrokeRoundRectOp : public PaintOp {
  StrokeRoundRectOp(const Rect &_rect, const Color &_color,
                    const CornerRadius &_radius, int _width, bool _dash)
      : PaintOp(PaintOpType::StrokeRoundRect), rect(_rect), color(_color),
        radius(_radius), width(_width), dash(_dash) {}
  Rect rect;
  Color color;
  CornerRadius radius;
  int width;
  bool dash;
};

struct DrawStringOp : public PaintOp {
  DrawStringOp(sk_sp<SkTextBlob> _blob, const Color &_color, int _x, int _y)
      : PaintOp(PaintOpType::DrawString), blob(_blob), color(_color), x(_x),
        y(_y) {}

  sk_sp<SkTextBlob> blob;
  Color color;
  float x; // SkScalar
  float y;
  // PaintFlags flags;
};

struct DrawBitmapOp : public PaintOp {
  DrawBitmapOp(std::shared_ptr<IRenderBitmap> _bitmap, DrawBitmapParam *_param)
      : PaintOp(PaintOpType::DrawBitmap), bitmap(_bitmap), param(*_param) {}
  
  std::shared_ptr<IRenderBitmap> bitmap;
  DrawBitmapParam param;
};

struct DumpToImageOp : public PaintOp {
  DumpToImageOp(const char *_path)
      : PaintOp(PaintOpType::DumpToImage), path(_path) {}
  std::string path;
};

struct SwapChainOp : public PaintOp {
  SwapChainOp(slot<void()> &&_callback)
    : PaintOp(PaintOpType::SwapChain), callback(std::move(_callback)) {}

  slot<void()> callback;
};

// struct Upload2GpuOp : public PaintOp {
//   Upload2GpuOp()
//       : PaintOp(PaintOpType::Upload2Gpu) { }
// };

struct RenderOnThreadOp : public PaintOp {
  RenderOnThreadOp(slot<void(IRenderTarget *)> &&_callback)
      : PaintOp(PaintOpType::RenderOnThread), callback(std::move(_callback)) {}

  slot<void(IRenderTarget *)> callback;
};

struct AsyncTaskCommand : public PaintOp {
  AsyncTaskCommand(slot<void()> &&_callback)
    : PaintOp(PaintOpType::AsyncTask), callback(std::move(_callback)) {}

  slot<void()> callback;
};

struct SyncLayerPropertiesCommand : public PaintOp {
  SyncLayerPropertiesCommand()
    : PaintOp(PaintOpType::SyncLayerProperties) {}
};

} // namespace ui
#endif