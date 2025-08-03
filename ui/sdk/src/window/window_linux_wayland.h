#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_WAYLAND_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_WAYLAND_H_

#include "gpu/include/api.h"
#include "linux/display_wayland.h"
#include "window.h"
#include <string>

namespace ui {

// 窗口surface 内存
class WaylandSurfaceSharedMemory {
public:
  WaylandSurfaceSharedMemory();
  ~WaylandSurfaceSharedMemory();

  bool Alloc(int width, int height);
  void Free();

  int fd() { return m_shm_fd; }
  void *data() { return m_shm_data; }
  int size() { return m_size; }

private:
  int m_shm_fd = -1;
  void *m_shm_data = nullptr;
  int m_size = 0;
};

enum WaylandVisibleState { Hidden, Minimized, Visible, Maximized };

class WindowPlatformLinuxWayland : public WindowPlatform,
                                   public ui::IGpuCompositorWindowWayland,
                                   public ISurfacePointerCallback {
public:
  WindowPlatformLinuxWayland(ui::Window &w);
  ~WindowPlatformLinuxWayland();

  // WindowPlatform
  virtual IGpuCompositorWindow *GetGpuCompositorWindow() override {
    return static_cast<IGpuCompositorWindow *>(this);
  }

  void Initialize() override;
  void Release() override;
  bool Create(CreateWindowParam &param) override;
  WINDOW_HANDLE GetWindowHandle() override;
  void SetTitle(const char *title) override;
  void Show(bool active=true) override;
  void Hide() override;
  void GetClientRect(Rect *prect) override;
  void GetWindowRect(Rect *prect) override;

  void UpdateNonClientRegion(Rect* region) override;
  void GetMonitorWorkArea(Rect* rect) override;
  void SetWindowPos(int x, int y, int w, int h,
                    SetPositionFlags flags) override;
  // void Invalidate(const Rect *prect) override;
  bool IsChildWindow() override;
  bool IsWindowVisible() override;
  void Commit(IRenderTarget *pRT, const Rect *prect, int count) override;

public:
  void Destroy();
  void Activate();

public:
  void on_xdg_surface_configure(struct xdg_surface *xdg_surface);
  void on_xdg_toplevel_configure(struct xdg_toplevel *xdg_toplevel,
                                 int32_t width, int32_t height,
                                 struct wl_array *states);
  void on_xdg_toplevel_close(struct xdg_toplevel *xdg_toplevel);

  void on_size();
  void on_visible_state_changed(WaylandVisibleState old);

protected:
  // ISurfacePointerCallback
  void on_pointer_enter(wl_fixed_t surface_x, wl_fixed_t surface_y) override;
  void on_pointer_leave() override;
  void on_pointer_motion(uint32_t time, wl_fixed_t x, wl_fixed_t y) override;
  void on_pointer_button(uint32_t serial, uint32_t time, uint32_t button,
                         uint32_t state, wl_fixed_t x, wl_fixed_t y) override;

  // IGpuCompositorWindowWayland
  struct wl_display *GetWaylandDisplay() override;
  struct wl_surface *GetWaylandSurface() override;

private:
  struct wl_buffer *create_shm_buffer(int width, int height, uint32_t format);
  void create_surface();
  void destroy_surface();
  void create_toplevel();
  void destroy_toplevel();

private:
  ui::Window &m_ui_window;

  WaylandDisplay m_display;

  struct wl_surface *m_surface = nullptr;
  struct xdg_surface *m_xdg_surface = nullptr;
  struct xdg_toplevel *m_xdg_toplevel = nullptr;

  // 窗口图像缓存
  WaylandSurfaceSharedMemory m_shm;

  // window state:
  WaylandVisibleState m_visible = WaylandVisibleState::Hidden;
  std::string m_title_utf8;
  unsigned short m_width = 0;
  unsigned short m_height = 0;
};

} // namespace ui

#endif