#include "display.h"
#include <assert.h>

namespace ui {

X11Display::~X11Display() { Destroy(); }

void X11Display::Init() {
  m_display = XOpenDisplay(NULL);
  assert(m_display);

  m_screen_number = DefaultScreen(m_display);
  m_wm_delete = XInternAtom(m_display, "WM_DELETE_WINDOW", True);
}
void X11Display::Destroy() {
  if (m_display) {
    XCloseDisplay(m_display);
    m_display = NULL;
  }
  m_screen_number = 0;
}

Atom &X11Display::GetWMDelete() { return m_wm_delete; }

unsigned long X11Display::GetWhite() {
  return WhitePixel(m_display, m_screen_number);
}
unsigned long X11Display::GetBlack() {
  return BlackPixel(m_display, m_screen_number);
}

int X11Display::GetScreenWidth() {
  return DisplayWidth(m_display, m_screen_number);
}
int X11Display::GetScreenHeight() {
  return DisplayHeight(m_display, m_screen_number);
}
::Window X11Display::GetDefaultRootWindow() {
  return DefaultRootWindow(m_display);
}

} // namespace ui