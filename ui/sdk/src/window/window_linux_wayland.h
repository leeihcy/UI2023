#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_WAYLAND_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_WAYLAND_H_

#include "linux/display_wayland.h"
#include "window.h"
#include <string>

namespace ui {
class WindowPlatformLinuxWayland : public WindowPlatform {
public:
  WindowPlatformLinuxWayland(ui::Window &w);
  ~WindowPlatformLinuxWayland();

  void Initialize() override;
  void Release() override;
  bool Create(const Rect &rect) override;
  WINDOW_HANDLE GetWindowHandle() override;
  void SetTitle(const char *title) override;
  void Show() override;
  float GetScaleFactor() override;
  void GetClientRect(Rect *prect) override;
  void GetWindowRect(Rect *prect) override;
  void SetWindowPos(int x, int y, int w, int h,
                    SetPositionFlags flags) override;
  void Invalidate(const Rect *prect) override;
  bool IsChildWindow() override;
  bool IsWindowVisible() override;
  void Commit(IRenderTarget *pRT, const Rect *prect, int count) override;

public:
  void _on_xdg_surface_configure(struct xdg_surface *xdg_surface);

private:
  struct wl_buffer *create_shm_buffer(int width, int height, uint32_t format);

private:
  ui::Window &m_ui_window;

  WaylandDisplay m_display;

  struct wl_surface *m_surface = nullptr;
  struct xdg_surface *m_xdg_surface = nullptr;
  struct xdg_toplevel *m_xdg_toplevel = nullptr;
};

} // namespace ui

#endif