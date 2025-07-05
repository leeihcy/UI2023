#ifndef _UI_GPU_INCLUDE_API_H_
#define _UI_GPU_INCLUDE_API_H_

#include "gpu/include/interface.h"
#include "sdk/include/util/rect.h"
#include <wayland-client-core.h>

#if defined(OS_LINUX)
struct wl_display;
struct wl_surface;
#endif

namespace ui {

enum {
  TILE_SIZE = 256, // 纹理分块大小
};

struct IGpuLayer;
class GpuLayerCommitContext;

enum GpuCompositorWindowType {
  Unknown,
  WindowsHWND,
  MacOSNSView,
  LinuxX11,
  LinuxWayland
};
struct IGpuCompositorWindow {
  virtual GpuCompositorWindowType GetType() = 0;
  virtual void GetWindowSize(int* w, int* h) = 0;
};

#if defined(OS_WIN)
struct IGpuCompositorWindowHWND : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() { return GpuCompositorWindowType::WindowsHWND; }
  virtual HWND GetHWND() = 0;
}
#elif defined(OS_MAC)
struct IGpuCompositorWindowNSView : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() { return GpuCompositorWindowType::MacOSNSView; }
  virtual NSView* GetNSWindowRootView() = 0;
}
#elif defined(OS_LINUX)
struct IGpuCompositorWindowWayland : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() { return GpuCompositorWindowType::LinuxWayland; }

  virtual wl_display* GetWaylandDisplay() = 0;
  virtual wl_surface* GetWaylandSurface() = 0;
};

struct IGpuCompositorWindowX11 : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() { return GpuCompositorWindowType::LinuxWayland; }
};
#endif

struct IGpuCompositor {
  virtual ~IGpuCompositor() {}
  virtual bool Initialize(IGpuCompositorWindow*) = 0;
  virtual void Release() = 0;
  virtual IGpuLayer *CreateLayerTexture() = 0;
  virtual void SetRootLayerTexture(IGpuLayer *p) = 0;

  virtual bool BeginCommit(GpuLayerCommitContext*) = 0;
  virtual void EndCommit(GpuLayerCommitContext*) = 0;
  virtual void Resize(int nWidth, int nHeight) = 0;
  // virtual int GetWidth() = 0;
  // virtual int GetHeight() = 0;
};

struct UploadGpuBitmapInfo {
  int width;
  int height;
  int bpp;
  int pitch;
  unsigned char *pFirstLineBits;
  bool hasAlphaChannel;

  Rect *prcArray;
  unsigned int nCount;
};

class GpuLayerCommitContext {
public:
  GpuLayerCommitContext();
  ~GpuLayerCommitContext();

  void SetOffset(int x, int y);
  void ClipRect(Rect *prc);
  void SetClipRect(Rect *prc);
  // void  MultiMatrix(float* matrix16);
  void MultiAlpha(unsigned char alpha);

public:
  void* m_data = nullptr;

  int m_xOffset;
  int m_yOffset;

  Rect m_rcClip;
  float m_fAlpha;

  bool m_bTransformValid;
  float m_matrixTransform[4][4];
};

inline GpuLayerCommitContext::GpuLayerCommitContext() {
  m_xOffset = 0;
  m_yOffset = 0;

  m_rcClip.SetEmpty();

  m_fAlpha = 1.0f;
  m_bTransformValid = false;
  memset(&m_matrixTransform, 0, sizeof(m_matrixTransform));
  m_matrixTransform[0][0] = m_matrixTransform[1][1] = m_matrixTransform[2][2] =
      m_matrixTransform[3][3] = 1;
}

inline GpuLayerCommitContext::~GpuLayerCommitContext() {}

// void  GpuLayerCommitContext::OffsetBy(int x, int y)
// {
// 	m_xOffset += x;
// 	m_yOffset += y;
// }

inline void GpuLayerCommitContext::SetOffset(int x, int y) {
  m_xOffset = x;
  m_yOffset = y;
}

inline void GpuLayerCommitContext::ClipRect(Rect *prc) {
  if (!prc)
    return;

  m_rcClip.Intersect(*prc, &m_rcClip);
}

inline void GpuLayerCommitContext::SetClipRect(Rect *prc) {
  if (prc)
    m_rcClip.CopyFrom(*prc);
  else
    m_rcClip.SetEmpty();
}

inline void GpuLayerCommitContext::MultiAlpha(unsigned char alpha) {
  if (alpha == 255)
    return;

  if (alpha == 0) {
    m_fAlpha = 0;
    return;
  }

  m_fAlpha *= alpha / 255.0f;
}

struct IGpuLayer {
  virtual void Release() = 0;

#if 0
	void  Translation(float xPos, float yPos, float zPos);
	void  TranslationBy(float xPos, float yPos, float zPos);
    void  Rotate(float xRotate, float yRotate, float zRotate);
    void  RotateBy(float xRotate, float yRotate, float zRotate);
#endif
  virtual void OnBeginCommit(GpuLayerCommitContext *ctx) = 0;
  virtual void Compositor(GpuLayerCommitContext *pContext,
                          float *pMatrixTransform) = 0;

  virtual void UploadHBITMAP(UploadGpuBitmapInfo &info) = 0;
  virtual void Resize(int nWidth, int nHeight) = 0;
};

// extern "C" UICOMPOSITOR_API IRenderLayerTransform2*  CreateHard3DTransform();

extern "C" {
UIGPUAPI bool GpuStartup();
UIGPUAPI void GpuShutdown();
UIGPUAPI IGpuCompositor *CreateGpuComposition(IGpuCompositorWindow*);
UIGPUAPI void GpuUnitTest();
}

} // namespace ui

#endif