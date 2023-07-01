#include "window_linux.h"
#include <string.h>

namespace ui {

void WindowPlatformLinux::Initialize() { m_display.Init(); }

void WindowPlatformLinux::Release() { m_display.Destroy(); }

void WindowPlatformLinux::Create(const Rect &rect) {
  ::Window window = XCreateSimpleWindow(
      m_display, m_display.GetDefaultRootWindow(), rect.x, rect.y, rect.width,
      rect.height, 300, m_display.GetBlack(), m_display.GetWhite());
  
  // 必须再显示设置一次窗口位置，否则窗口一会坐标变成（0,0）
  XSizeHints hints;
  memset(&hints, 0, sizeof(hints));
  hints.x = rect.x;
  hints.y = rect.y;
  hints.width = rect.width;
  hints.height = rect.height;
  hints.flags |= USPosition | USSize | PPosition | PSize;
  XSetWMNormalHints(m_display, window, &hints);

  Attach(window);
}

void WindowPlatformLinux::Attach(::Window window) {
  if (!window) {
    return;
  }

  // this->display = display_wrap.GetDisplay();
  // this->m_root_window = DefaultRootWindow(display);

  this->m_window = window;
  // s_window_mapper.RegisterWindow(this);

  // this->initGC();
  // this->initEvent();

  // RECT rc;
  // GetWindowRect(&rc);
  // resize(rc.width(), rc.height());

  // XFlush(display);
  // this->on_create();
}

void WindowPlatformLinux::Show()
{
    XMapWindow(m_display, m_window);
    XFlush(m_display);
}

void WindowPlatformLinux::Hide() 
{
    XUnmapWindow(m_display, m_window);
}

} // namespace ui