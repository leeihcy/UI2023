#include "display_wayland.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


namespace ui {
struct wl_display *WaylandDisplay::m_display = nullptr;
struct wl_compositor *WaylandDisplay::m_compositor = nullptr;

struct wl_compositor *WaylandDisplay::m_wl_compositor = nullptr;
struct wl_shm *WaylandDisplay::m_wl_shm = nullptr;
struct xdg_wm_base *WaylandDisplay::m_xdg_wm_base = nullptr;


static const struct wl_registry_listener registry_listener = {
    .global = WaylandDisplay::_on_registry_handle_global,
    .global_remove = NULL,
};

bool WaylandDisplay::Init() {
  if (m_display) {
    return true;
  }

  // 连接到 Wayland 服务器
  m_display = wl_display_connect(NULL);
  if (!m_display) {
    printf("wl_display_connect failed.\n");
    assert(false && "无法连接到 Wayland 显示服务器");
    return false;
  }

  struct wl_registry *registry = wl_display_get_registry(m_display);
  wl_registry_add_listener(registry, &registry_listener, NULL);
  wl_display_roundtrip(m_display);

  if (!m_wl_compositor) {
    assert(false && "缺少 Wayland 接口: wl_compositor");
    printf("miss wayland wl_compositor\n");
    return false;
  }
  if (!m_xdg_wm_base) {
    assert(false && "缺少 Wayland 接口: xdg_wm_base");
    printf("miss wayland xdg_wm_base\n");
    return false;
  }
  if (!m_wl_shm) {
    assert(false && "缺少 Wayland 接口: wl_shm");
    printf("miss wayland wl_shm\n");
    return false;
  }
  return true;
}

void WaylandDisplay::Destroy() {
    // wl_display_disconnect(m_display);
}


void WaylandDisplay::_on_registry_handle_global(void *data,
                                                struct wl_registry *registry,
                                                uint32_t id,
                                                const char *interface,
                                                uint32_t version) {
  if (strcmp(interface, "wl_compositor") == 0) {
    WaylandDisplay::m_wl_compositor = (struct wl_compositor *)wl_registry_bind(
        registry, id, &wl_compositor_interface, 4);
  } else if (strcmp(interface, "xdg_wm_base") == 0) {
    WaylandDisplay::m_xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(
        registry, id, &xdg_wm_base_interface, 1);
  } else if (strcmp(interface, "wl_shm") == 0) {
    WaylandDisplay::m_wl_shm =
        (struct wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
  }
}

bool WaylandDisplay::IsWaylandDesktopEnviroment() {
  // 判断当前桌面类型
  const char* session_type = getenv("XDG_SESSION_TYPE");
  bool is_wayland = false;
  if (session_type && strcmp(session_type, "wayland")==0) {
    is_wayland = true;
  } else if (getenv("WAYLAND_DISPLAY")) {
    is_wayland = true;
  }

  return is_wayland;
}

} // namespace ui