#ifndef _UI_SRK_SRC_THREAD_PAINT_OP_H_
#define _UI_SRK_SRC_THREAD_PAINT_OP_H_
#include "include/util/color.h"
#include "include/util/rect.h"
#include "src/graphics/record/record_render_target.h"
#include <memory>

#include "include/util/rect_region.h"
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
  SetDirtyRegion,
  SetOrigin,
  OffsetOrigin,
  ClipRoundRect,
  ClipRect,
  Clear,
  DrawRect,
  DrawString,
  DrawBitmap,
  CreateSwapChain,
  SwapChain,
  DumpToImage,
  Upload2Gpu,
  RenderOnThread,
  
  RenderOpEnd,
  
  // --------------------------

  // post command for thread
  PostCommandStart,
  RemoveKey,
  PostCommandEnd,
};

struct PaintOp {
  PaintOp(PaintOpType t) : type(t) {}
  void processOnRenderThread(IRenderTarget* rt);

  void *key = nullptr;
  PaintOpType type;
};


struct BeginDrawOp : public PaintOp {
public:
  BeginDrawOp(float _scale) : PaintOp(PaintOpType::BeginDraw), scale(_scale) {}

  float scale;
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

struct SetDirtyRegionOp : public PaintOp {
  SetDirtyRegionOp(const DirtyRegion& _dirty_region)
      : PaintOp(PaintOpType::SetDirtyRegion), dirty_region(_dirty_region){}
  DirtyRegion dirty_region;
};

struct ClipRoundRectOp : public PaintOp {
  ClipRoundRectOp(const Rect& _rect, int _radius)
      : PaintOp(PaintOpType::ClipRoundRect), rect(_rect), radius(_radius){}
  Rect rect;
  int radius;
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

struct ClearOp : public PaintOp {
  ClearOp(const Rect &_rect) : PaintOp(PaintOpType::Clear), rect(_rect) {}
  Rect rect;
};

struct DrawRectOp : public PaintOp {
  DrawRectOp(const Rect &_rect, const Color &_color)
      : PaintOp(PaintOpType::DrawRect), rect(_rect), color(_color) {}
  Rect rect;
  Color color;
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
  DrawBitmapOp(std::shared_ptr<IRenderBitmap> _bitmap, DRAWBITMAPPARAM *_param)
      : PaintOp(PaintOpType::DrawBitmap), bitmap(_bitmap), param(*_param) {}

  std::shared_ptr<IRenderBitmap> bitmap;
  DRAWBITMAPPARAM param;
};

struct DumpToImageOp : public PaintOp {
  DumpToImageOp(const char *_path)
      : PaintOp(PaintOpType::DumpToImage), path(_path) {}
  std::string path;
};

struct CreateSwapChainOp : public PaintOp {
  CreateSwapChainOp(bool _is_hardware)
      : PaintOp(PaintOpType::CreateSwapChain), is_hardware(_is_hardware){ }

  bool is_hardware;
};

struct SwapChainOp : public PaintOp {
  SwapChainOp(slot<void()> &&_callback)
    : PaintOp(PaintOpType::SwapChain), callback(std::move(_callback)) {}

  slot<void()> callback;
};

struct Upload2GpuOp : public PaintOp {
  Upload2GpuOp()
      : PaintOp(PaintOpType::Upload2Gpu) { }
};

struct RenderOnThreadOp : public PaintOp {
  RenderOnThreadOp(slot<void(IRenderTarget *)> &&_callback)
      : PaintOp(PaintOpType::RenderOnThread), callback(std::move(_callback)) {}

  slot<void(IRenderTarget *)> callback;
};

} // namespace ui
#endif