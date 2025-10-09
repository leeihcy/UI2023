#ifndef _UI_SDK_SRC_GRAPHICS_RECORD_RECORD_RENDER_TARGET_H_
#define _UI_SDK_SRC_GRAPHICS_RECORD_RECORD_RENDER_TARGET_H_
#include "include/interface/renderlibrary.h"
#include "include/util/color.h"
#include "src/graphics/clip_origin.h"
#include "src/graphics/skia/skia_render.h"
#include <memory>

namespace ui {
class RenderThread;
struct PaintOp;

// 只记录渲染操作，将操作转换给RenderThread
class RecordRenderTarget : public IRenderTarget {
public:
  RecordRenderTarget();

  ~RecordRenderTarget() override;
  void Release() override;
  GRAPHICS_RENDER_LIBRARY_TYPE Type() override {
    return GRAPHICS_RENDER_LIBRARY_TYPE_SKIA_RECORD;
  }
  bool BeginDraw(float scale) override;
  void EndDraw() override;
  void Clear(const Rect& rect) override;
  bool Resize(unsigned int nWidth, unsigned int nHeight) override;
  void* GetHandle() override;
  
  void Save() override;
  void Restore() override;
  void ClipRoundRect(const Rect& rect, int radius) override;
  void ClipRect(const Rect& rect) override;

  void CreateSwapChain(bool is_hardware, IGpuCompositor*) override;
  bool SwapChain(slot<void()>&& callback) override;

  void DumpToImage(const char *szPath) override;
  void Upload2Gpu(Rect *prcArray, int nCount,
                  float scale) override;
  bool GetFrontFrameBuffer(FrameBufferWithReadLock* fb) override;
  void RenderOnThread(slot<void(IRenderTarget*)>&& callback) override;

  void SetDirtyRegion(const DirtyRegion& dirty_region) override;
  void PushRelativeClipRect(const Rect &rect) override;
  void PopRelativeClipRect() override;
  void SetOrigin(int x, int y) override;
  void OffsetOrigin(int x, int y) override;
  bool IsRelativeRectInClip(const Rect &rect) override;

  void Render2Target(IRenderTarget *pDst, Render2TargetParam *pParam) override;
  void DrawRect(const Rect& rc, const Color& color) override;
  void DrawBitmap(std::shared_ptr<IRenderBitmap> bitmap, DRAWBITMAPPARAM *param) override;
  void DrawString(const DrawTextParam &param) override;
  void _DrawString2(void* text_blob, const Color& color, float x, float y) override;

private:
  void addPaintOp(std::unique_ptr<PaintOp>&& paint_op);

private:
  float m_scale = 0.f;
  ClipOriginImpl m_clip_origin_impl;

  // 对应渲染线程上的rt对象
  // 由record rt来维护该对象的生命周期
  // 释放时，需要确保渲染线程不再使用，由rt线程释放。
  std::unique_ptr<SkiaRenderTarget> m_rt_async;
};

} // namespace ui

#endif // _UI_SDK_SRC_GRAPHICS_RECORD_RECORD_RENDER_TARGET_H_