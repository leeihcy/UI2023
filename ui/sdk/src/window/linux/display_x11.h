#ifndef _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_X11_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_X11_H_

/* needed to break into 'Display' struct internals. */
#define XLIB_ILLEGAL_ACCESS

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace ui {

struct XEventDispatcher {
    virtual void OnXEvent(const XEvent& event) = 0;
};


class X11Display {
public:
  ~X11Display();
  
  static void Init();
  static void Destroy();

  int GetScreenWidth();
  int GetScreenHeight();
  unsigned long GetWhite();
  unsigned long GetBlack();
  ::Window GetDefaultRootWindow();

  int fd();

  Atom WM_DELETE();
  Atom WM_TAKE_FOCUS();

  Display *GetDisplay() { return m_display; }
  operator Display *() { return m_display; }

  void BindXEventDispatcher(::Window window, XEventDispatcher* dispatcher);
  XEventDispatcher* FindDispatcher(::Window window);

  float GetMonitorScaleFactor();

private:
  static Display *m_display;
  static XContext m_context;
  static int m_screen_number;
  static Atom m_wm_delete;
  static Atom m_wm_take_focus;
};

} // namespace ui
#endif