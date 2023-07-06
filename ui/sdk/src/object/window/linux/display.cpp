#include "display.h"

#include <X11/Xresource.h> // XUniqueContext
#include <assert.h>

namespace ui {

Display *X11Display::m_display = nullptr;
XContext X11Display::m_context = 0;
int X11Display::m_screen_number = 0;

Atom X11Display::m_wm_delete = 0;
Atom X11Display::m_wm_take_focus = 0;

X11Display::~X11Display() {
  /*Destroy();
   */
}

void X11Display::Init() {
  if (m_display) {
    return;
  }
  m_display = XOpenDisplay(NULL);
  assert(m_display);

  // 用于::Window -> ClassPtr转换
  m_context = XUniqueContext();

  m_screen_number = DefaultScreen(m_display);

  m_wm_delete = XInternAtom(m_display, "WM_DELETE_WINDOW", True);
  m_wm_take_focus = XInternAtom(m_display, "WM_TAKE_FOCUS", True);
}

void X11Display::Destroy() {
  if (m_display) {
    XCloseDisplay(m_display);
    m_display = NULL;
  }
  m_screen_number = 0;
}

int X11Display::fd() { return m_display->fd; }

Atom X11Display::WM_DELETE() { return m_wm_delete; }
Atom X11Display::WM_TAKE_FOCUS() { return m_wm_take_focus; }

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

void X11Display::BindXEventDispatcher(::Window window,
                                      XEventDispatcher *dispatcher) {
  XSaveContext(m_display, window, m_context, (XPointer)dispatcher);
}
XEventDispatcher *X11Display::FindDispatcher(::Window window) {
  XPointer pointer = nullptr;
  XFindContext(m_display, window, m_context, &pointer);
  if (!pointer) {
    return nullptr;
  }
  return (XEventDispatcher *)pointer;
}

} // namespace ui