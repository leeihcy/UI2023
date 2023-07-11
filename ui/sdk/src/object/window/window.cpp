#include "window.h"
#include <SkColorSpace.h>
#include <assert.h>

#if defined(OS_MAC)
#include "window_mac.h"
#elif defined(OS_LINUX)
#include "window_linux.h"
#endif

namespace ui {

Window::Window() {}
Window::~Window() {
  if (m_platform) {
    m_platform->Release();
    delete m_platform;
    m_platform = nullptr;
  }
}

void Window::Create(const Rect &rect) {
#if defined(OS_WIN)
#elif defined(OS_MAC) 
  m_platform = new WindowPlatformMac();
#elif defined(OS_LINUX)
  m_platform = new WindowPlatformLinux();
#else
  assert(false);
#endif
  m_platform->Initialize(this);
  m_platform->Create(rect);
}

void Window::Show() {
  if (m_platform) {
    m_platform->Show();
  }
}

void Window::onSize(int width, int height) {
  if (m_width == width && m_height == height) {
    return;
  }
  m_width = width;
  m_height = height;
  
  SkImageInfo info = SkImageInfo::Make(width, height, kBGRA_8888_SkColorType,
                                       kPremul_SkAlphaType, nullptr);
  SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);
  m_sksurface = SkSurface::MakeRaster(info, &surfaceProps);
}

void Window::onClose() {}
void Window::onDestroy() { m_signal_destroy.emit(); }

void Window::onPaint(Rect *dirty) {
  if (!m_sksurface) {
    return;
  }

  SkCanvas *canvas = m_sksurface->getCanvas();
  on_erase_bkgnd(canvas);
  on_paint(canvas);

  m_sksurface->flushAndSubmit();
  swap_buffer();
}

void Window::on_paint(SkCanvas *canvas) {}
void Window::on_erase_bkgnd(SkCanvas *canvas) {
    // canvas->clear(SK_ColorWHITE);
    canvas->clear(SK_ColorRED);
}
void Window::swap_buffer() {
    m_platform->Submit(m_sksurface);
}

} // namespace ui