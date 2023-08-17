#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_H_
#include "linux/display.h"
#include "window.h"

#include <string>

namespace ui {
class WindowPlatformLinux : public WindowPlatform, public XEventDispatcher {
public:
  WindowPlatformLinux(ui::Window &w);
  ~WindowPlatformLinux();
  void Initialize() override;

  bool Create(const Rect &rect) override;
  bool CreateTransparent(const Rect &rect);
  void Destroy();

  void SetWindowTitle(const char *title);
  std::string GetWindowTitle();
  void SetBorderless(bool no_border = true);
  void SetMinMaxSize(int wMin, int hMin, int wMax, int hMax);
  void SetWindowRect(int x, int y, int width, int height);
  void GetClientRect(Rect *prect) override;
  void GetWindowRect(Rect *prect) override;
  void CenterWindow();
  void InvalidateRect(Rect* prect) override;
  void ValidateRect(Rect* prect) override;
  bool IsChildWindow() override;
  bool IsWindowVisible() override;
  ::Window GetParentWindow();

  void Attach(::Window window);
  ::Window Detach();

  void Show() override;
  void Hide();

  void Submit(sk_sp<SkSurface> sksurface) override;
  void Invalidate();

  // XEventDispatcher
  void OnXEvent(const XEvent &event) override;

private:
  void initGC();
  void initEvent();

private:
  ui::Window &m_ui_window;

  X11Display m_display;
  ::Window m_window = 0;
  GC m_gc = 0;
};

} // namespace ui

#endif