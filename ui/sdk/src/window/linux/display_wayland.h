#ifndef _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_WAYLAND_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_LINUX_DISPLAY_WAYLAND_H_

#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

namespace ui {

class WaylandDisplay {
public:
  static bool Init();
  static void Destroy();

  static bool IsWaylandDesktopEnviroment();
  
public:
  static void _on_registry_handle_global(void *data, struct wl_registry *registry,
                                 uint32_t id, const char *interface,
                                 uint32_t version);

  struct wl_display * get_display() { return m_display; }                               
  struct wl_compositor * get_wl_compositor() { return m_wl_compositor; }
  struct wl_shm * get_wl_shm() { return m_wl_shm; }
  struct xdg_wm_base * get_xdg_wm_base() { return m_xdg_wm_base; }

private:
  static struct wl_display *m_display;
  static struct wl_compositor *m_compositor;

  static struct wl_compositor *m_wl_compositor;
  static struct wl_shm *m_wl_shm;
  static struct xdg_wm_base *m_xdg_wm_base;
};

} // namespace ui

#endif