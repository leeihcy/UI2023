#pragma once
#include <vector>
#include <shared_mutex>

#include "include/util/rect_region.h"
#include "third_party/skia/src/include/core/SkCanvas.h"
#include "third_party/skia/src/include/core/SkColorSpace.h"
#include "third_party/skia/src/include/core/SkSurface.h"

#include "include/interface/renderlibrary.h"
#include "src/graphics/clip_origin.h"

namespace ui {
class RenderBuffer;

class SkiaRenderTarget : public IRenderTarget {
public:
  SkiaRenderTarget();
  ~SkiaRenderTarget();
  void Release() override;
  GRAPHICS_RENDER_LIBRARY_TYPE Type() override {
    return GRAPHICS_RENDER_LIBRARY_TYPE_SKIA;
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

  void CreateSwapChain(bool is_hardware, IGpuCompositor* gpu_compositor) override;
  bool SwapChain(slot<void()>&& callback) override;
  void DumpToImage(const char *szPath) override;
  void Upload2Gpu(Rect *prcArray, int nCount,
                          float scale) override;
  bool GetFrontFrameBuffer(FrameBufferWithReadLock* fb) override;
  void RenderOnThread(slot<void(IRenderTarget*)>&& callback) override;
  void SetDirtyRegion(const DirtyRegion& dirty_region) override;
  const DirtyRegion& GetDirtyRegion();
  void PushRelativeClipRect(const Rect&) override;
  void PopRelativeClipRect() override;
  void SetOrigin(int x, int y) override;
  void OffsetOrigin(int x, int y) override;

  void Render2Target(IRenderTarget *pDst,
                             Render2TargetParam *pParam) override;
  void DrawRect(const Rect& rc, const Color& color) override;
  void DrawBitmap(std::shared_ptr<IRenderBitmap> bitmap,
                          DRAWBITMAPPARAM *param) override;
  void DrawString(const DrawTextParam &param) override;
  void _DrawString2(void* text_blob, const Color& color, float x, float y) override;

  bool IsRelativeRectInClip(const Rect& rect) override;
public:
  SkSurface *GetSkiaSurface() { return m_sksurface.get(); }

protected:
  void frames_sync_size();
  void frames_sync_dirty();

  void update_clip_rgn();
  void upload_2_gpu();
  void resize_gpu_layer(unsigned int width, unsigned int height);
  void render_to_surface(SkSurface* source, SkSurface* target, Render2TargetParam *param);
  
  sk_sp<SkSurface> create_surface(unsigned int width, unsigned int height);

protected:
  sk_sp<SkSurface> m_sksurface; 
  ClipOriginImpl m_clip_origin_impl;
  float m_scale = 1.0f;

  // 用于标记本轮begin/end draw期间，是否被调用了Resize
  // 用于swap chain同步size
  int m_width = 0;
  int m_height = 0;
  bool m_resized_flags = false;

  // software backend
  bool m_enable_software_backend = false;
  DirtyRegion m_last_dirty_region;

  // m_sksurface相当于back，用于绘制。front仅用于UI线程。
  // 软件合成(ui线程读取)
  struct Main {
    sk_sp<SkSurface> m_sksurface_front; 
    std::shared_mutex m_mutex;
  } main;

  // gpu backend
  // 硬件合成
  bool m_enable_hardware_backend = false;
  IGpuLayer *m_gpu_texture = nullptr;
};

} // namespace ui
