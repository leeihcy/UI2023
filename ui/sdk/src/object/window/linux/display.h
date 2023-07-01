#ifndef _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace ui {

class X11Display {
public:
  ~X11Display();
  void Init();
  void Destroy();

  int GetScreenWidth();
  int GetScreenHeight();
  unsigned long GetWhite();
  unsigned long GetBlack();
  Atom &GetWMDelete();
  ::Window GetDefaultRootWindow();

  Display *GetDisplay() { return m_display; }
  operator Display* () { return m_display; }
  
private:
  int m_screen_number = 0;
  Display *m_display = nullptr;
  Atom m_wm_delete;
};

} // namespace ui
#endif