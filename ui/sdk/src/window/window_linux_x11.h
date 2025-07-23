#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_X11_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_X11_H_
#include "gpu/include/api.h"
#include "linux/display_x11.h"
#include "window.h"
#include <string>

namespace ui {
class WindowPlatformLinuxX11 : public WindowPlatform,
                               public IGpuCompositorWindowX11,
                               public XEventDispatcher {
public:
  WindowPlatformLinuxX11(ui::Window &w);
  ~WindowPlatformLinuxX11();

  // WindowPlatform
  IGpuCompositorWindow *GetGpuCompositorWindow() override {
    return static_cast<IGpuCompositorWindow *>(this);
  }

  void Initialize() override;

  bool Create(CreateWindowParam &param) override;
  WINDOW_HANDLE GetWindowHandle() override;
  bool CreateTransparent(const Rect &rect);
  void Destroy();

  void SetTitle(const char *title) override;
  std::string GetWindowTitle();
  void SetBorderless(bool no_border = true);
  void SetMinMaxSize(int wMin, int hMin, int wMax, int hMax);
  void SetWindowRect(int x, int y, int width, int height);
  void SetWindowPos(int x, int y, int w, int h,
                    SetPositionFlags flags) override;
  void GetClientRect(Rect *prect) override;
  void GetWindowRect(Rect *prect) override;
  void CenterWindow();
  // void Invalidate(const Rect *prect) override;
  bool IsChildWindow() override;
  bool IsWindowVisible() override;
  ::Window GetParentWindow();

  void Attach(::Window window);
  ::Window Detach();

  void Show() override;
  void Hide();

  void Commit(IRenderTarget *pRT, const Rect *prect, int count) override;

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