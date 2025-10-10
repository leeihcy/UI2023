#ifndef _UI_GPU_INCLUDE_API_H_
#define _UI_GPU_INCLUDE_API_H_

#include "gpu/include/interface.h"
#include "sdk/include/util/rect.h"
#include <memory>

#if defined(WS_WIN)
#elif defined(OS_LINUX)
struct wl_display;
struct wl_surface;
#elif defined(OS_MAC)
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
};

#if defined(OS_WIN)
struct IGpuCompositorWindowHWND : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() override { return GpuCompositorWindowType::WindowsHWND; }
  virtual void* GetHWND() = 0; // HWND
};
#elif defined(OS_MAC)
struct IGpuCompositorWindowNSView : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() override { return GpuCompositorWindowType::MacOSNSView; }
  virtual void* GetNSWindowRootView() = 0; // NSView*
};
#elif defined(OS_LINUX)
struct IGpuCompositorWindowWayland : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() override { return GpuCompositorWindowType::LinuxWayland; }

  virtual wl_display* GetWaylandDisplay() = 0;
  virtual wl_surface* GetWaylandSurface() = 0;
};

struct IGpuCompositorWindowX11 : public IGpuCompositorWindow {
  GpuCompositorWindowType GetType() override { return GpuCompositorWindowType::LinuxWayland; }
};
#endif

struct IGpuCompositor {
  virtual ~IGpuCompositor() {}
  virtual std::shared_ptr<IGpuLayer> CreateLayerTexture() = 0;

  virtual bool BeginCommit(GpuLayerCommitContext*) = 0;
  virtual void EndCommit(GpuLayerCommitContext*) = 0;
  virtual void Resize(int nWidth, int nHeight) = 0;
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

  // bool m_bTransformValid;
  // float m_matrixTransform[4][4];
};

inline GpuLayerCommitContext::GpuLayerCommitContext() {
  m_xOffset = 0;
  m_yOffset = 0;

  m_rcClip.SetEmpty();

  m_fAlpha = 1.0f;
  // m_bTransformValid = false;
  // memset(&m_matrixTransform, 0, sizeof(m_matrixTransform));
  // m_matrixTransform[0][0] = m_matrixTransform[1][1] = m_matrixTransform[2][2] =
  //     m_matrixTransform[3][3] = 1;
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
#if 0
	void  Translation(float xPos, float yPos, float zPos);
	void  TranslationBy(float xPos, float yPos, float zPos);
    void  Rotate(float xRotate, float yRotate, float zRotate);
    void  RotateBy(float xRotate, float yRotate, float zRotate);
#endif
  // virtual void OnBeginCommit(GpuLayerCommitContext *ctx) = 0;
  virtual void Compositor(GpuLayerCommitContext *pContext,
                          float *pMatrixTransform) = 0;

  virtual void UploadBitmap(UploadGpuBitmapInfo &info) = 0;
  virtual void Resize(int nWidth, int nHeight) = 0;
};

// extern "C" UICOMPOSITOR_API IRenderLayerTransform2*  CreateHard3DTransform();

enum GPU_STARTUP_STATE {
  NOT_START = -2,
  STARTING = -1,
  START_FAILED = 0,
  STARTED = 1
};

extern "C" {
UIGPUAPI bool GpuStartup();
UIGPUAPI void GpuShutdown();
UIGPUAPI GPU_STARTUP_STATE GetGpuStartupState();
UIGPUAPI void GpuUnitTest();
}
UIGPUAPI std::shared_ptr<IGpuCompositor> CreateGpuComposition(IGpuCompositorWindow*);


} // namespace ui

#endif