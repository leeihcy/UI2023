#ifndef _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_WAYLAND_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_WAYLAND_H_

#include "xdg-shell-client-protocol.h"
#include "xdg-output-unstable-v1-client-protocol.h"
#include "xdg-activation-v1-client-protocol.h"
#include <map>
#include <memory>
#include <wayland-client.h>

namespace ui {

struct ISurfacePointerCallback {
  virtual void on_pointer_enter(wl_fixed_t surface_x, wl_fixed_t surface_y) {}
  virtual void on_pointer_leave() {}
  virtual void on_pointer_motion(uint32_t time, wl_fixed_t x, wl_fixed_t y) {}
  virtual void on_pointer_button(uint32_t serial, uint32_t time, uint32_t button, uint32_t state,
                                 wl_fixed_t x, wl_fixed_t y) {}
};

class WaylandDisplayPrivate {
public:
  static WaylandDisplayPrivate &getInstance();

  bool Init();
  void Destroy();

  void BindSurface(struct wl_surface *, ISurfacePointerCallback *);
  void UnbindSurface(struct wl_surface *);
  ISurfacePointerCallback *MapSurface(struct wl_surface *);

public:
  void on_registry_handle_global(struct wl_registry *registry, uint32_t id,
                                 const char *interface, uint32_t version);
  void on_pointer_enter(struct wl_surface *surface, wl_fixed_t surface_x,
                        wl_fixed_t surface_y);
  void on_pointer_leave(struct wl_surface *surface);
  void on_pointer_motion(uint32_t time, wl_fixed_t x, wl_fixed_t y);
  void on_pointer_button(uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

  void on_wl_output_mode(struct wl_output *wl_output, uint32_t flags, int32_t width,
                         int32_t height, int32_t refresh);
protected:
  struct wl_display *m_display = nullptr;
  struct wl_compositor *m_compositor = nullptr;

  struct wl_compositor *m_wl_compositor = nullptr;
  struct wl_shm *m_wl_shm = nullptr;
  struct xdg_wm_base *m_xdg_wm_base = nullptr;
  struct wl_seat *m_wl_seat = nullptr;
  struct wl_pointer *m_wl_pointer = nullptr;
  struct wl_output *m_wl_output = nullptr;
  struct xdg_activation_v1* m_xdg_activation_v1 = nullptr;

#if 0
  struct zxdg_output_manager_v1* m_zxdg_output_manager_v1 = nullptr;
#endif
  friend class WaylandDisplay;

private:
  std::map<wl_surface *, ISurfacePointerCallback *> m_map_window;
  // 记录当前鼠标下的窗口。因为鼠标移动和点击时没有返回窗口参数。
  struct wl_surface *m_hover_surface = nullptr;
  int m_last_pointer_x = 0;
  int m_last_pointer_y = 0;

public:
  int m_output_width = 0;
  int m_output_height = 0;
};

class WaylandDisplay {
public:
  WaylandDisplay();
  ~WaylandDisplay();
  static bool IsWaylandDesktopEnviroment();

  struct wl_display *get_display();
  struct wl_compositor *get_wl_compositor();
  struct wl_shm *get_wl_shm();
  struct xdg_wm_base *get_xdg_wm_base();
  struct wl_seat *get_wl_seat();
  struct xdg_activation_v1* get_xdg_activation_v1();

  void BindSurface(struct wl_surface *, ISurfacePointerCallback *);
  void UnbindSurface(struct wl_surface *);

  void GetOutputSize(int* width, int* height) const;
};

} // namespace ui

#endif