#include "display_wayland.h"
#include "src/window/linux/xdg-shell-client-protocol.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <wayland-client-protocol.h>

namespace ui {

WaylandDisplayPrivate &WaylandDisplayPrivate::getInstance() {
  static WaylandDisplayPrivate s;
  return s;
}

void on_registry_handle_global(void *data, struct wl_registry *registry,
                               uint32_t id, const char *interface,
                               uint32_t version) {

  ((WaylandDisplayPrivate *)data)
      ->on_registry_handle_global(registry, id, interface, version);
}

static const struct wl_registry_listener registry_listener = {
    .global = on_registry_handle_global,
    .global_remove = NULL,
};

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base,
                             uint32_t serial) {
  xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void pointer_enter(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t surface_x, wl_fixed_t surface_y) {

  ((WaylandDisplayPrivate *)data)
      ->on_pointer_enter(surface, surface_x, surface_y);
}
void WaylandDisplayPrivate::on_pointer_enter(struct wl_surface *surface,
                                             wl_fixed_t surface_x,
                                             wl_fixed_t surface_y) {

  ISurfacePointerCallback *callback = MapSurface(surface);
  if (!callback) {
    return;
  }

  m_hover_surface = surface;
  m_last_pointer_x = surface_x;
  m_last_pointer_y = surface_y;

  callback->on_pointer_enter(surface_x, surface_y);
}

static void pointer_leave(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface) {
  ((WaylandDisplayPrivate *)data)->on_pointer_leave(surface);
}

void WaylandDisplayPrivate::on_pointer_leave(struct wl_surface *surface) {
  if (m_hover_surface == surface) {
    m_hover_surface = nullptr;
  }

  ISurfacePointerCallback *callback = MapSurface(surface);
  if (!callback) {
    return;
  }
  callback->on_pointer_leave();
}

static void pointer_motion(void *data, struct wl_pointer *pointer,
                           uint32_t time, wl_fixed_t x, wl_fixed_t y) {
  ((WaylandDisplayPrivate *)data)->on_pointer_motion(time, x, y);
}

void WaylandDisplayPrivate::on_pointer_motion(uint32_t time, wl_fixed_t x,
                                              wl_fixed_t y) {
  if (!m_hover_surface) {
    return;
  }

  ISurfacePointerCallback *callback = MapSurface(m_hover_surface);
  if (!callback) {
    return;
  }

  m_last_pointer_x = x;
  m_last_pointer_y = y;
  callback->on_pointer_motion(time, x, y);
}

static void pointer_button(void *data, struct wl_pointer *pointer,
                           uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state) {
  ((WaylandDisplayPrivate *)data)->on_pointer_button(time, button, state);
}

void WaylandDisplayPrivate::on_pointer_button(uint32_t time, uint32_t button,
                                              uint32_t state) {
  if (!m_hover_surface) {
    return;
  }

  ISurfacePointerCallback *callback = MapSurface(m_hover_surface);
  if (!callback) {
    return;
  }
  callback->on_pointer_button(time, button, state, m_last_pointer_x, m_last_pointer_y);
}

static struct wl_pointer_listener pointer_listener = {
    .enter = &pointer_enter,
    .leave = &pointer_leave,
    .motion = &pointer_motion,
    .button = &pointer_button,
};

bool WaylandDisplayPrivate::Init() {
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
  wl_registry_add_listener(registry, &registry_listener, this);
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

void WaylandDisplayPrivate::Destroy() {
  // wl_display_disconnect(m_display);
}

void WaylandDisplayPrivate::on_registry_handle_global(
    struct wl_registry *registry, uint32_t id, const char *interface,
    uint32_t version) {
  if (strcmp(interface, "wl_compositor") == 0) {
    m_wl_compositor = (struct wl_compositor *)wl_registry_bind(
        registry, id, &wl_compositor_interface, 4);
  } else if (strcmp(interface, "xdg_wm_base") == 0) {
    m_xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(
        registry, id, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(m_xdg_wm_base, &xdg_wm_base_listener, NULL);
  } else if (strcmp(interface, "wl_shm") == 0) {
    m_wl_shm =
        (struct wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    m_wl_seat =
        (struct wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 1);
    m_wl_pointer = (struct wl_pointer *)wl_seat_get_pointer(m_wl_seat);
    wl_pointer_add_listener(m_wl_pointer, &pointer_listener, this);
  }
}

WaylandDisplay::WaylandDisplay() {
  WaylandDisplayPrivate::getInstance().Init();
}
WaylandDisplay::~WaylandDisplay() {
  WaylandDisplayPrivate::getInstance().Destroy();
}

bool WaylandDisplay::IsWaylandDesktopEnviroment() {
  // 判断当前桌面类型
  const char *session_type = getenv("XDG_SESSION_TYPE");
  bool is_wayland = false;
  if (session_type && strcmp(session_type, "wayland") == 0) {
    is_wayland = true;
  } else if (getenv("WAYLAND_DISPLAY")) {
    is_wayland = true;
  }

  return is_wayland;
}

struct wl_display *WaylandDisplay::get_display() {
  return WaylandDisplayPrivate::getInstance().m_display;
}
struct wl_compositor *WaylandDisplay::get_wl_compositor() {
  return WaylandDisplayPrivate::getInstance().m_wl_compositor;
}
struct wl_shm *WaylandDisplay::get_wl_shm() {
  return WaylandDisplayPrivate::getInstance().m_wl_shm;
}
struct xdg_wm_base *WaylandDisplay::get_xdg_wm_base() {
  return WaylandDisplayPrivate::getInstance().m_xdg_wm_base;
}

void WaylandDisplayPrivate::BindSurface(struct wl_surface *k,
                                        ISurfacePointerCallback *v) {
  m_map_window[k] = v;
}
void WaylandDisplayPrivate::UnbindSurface(struct wl_surface *k) {
  m_map_window.erase(k);
  if (k == m_hover_surface) {
    m_hover_surface = nullptr;
  }
}

ISurfacePointerCallback *
WaylandDisplayPrivate::MapSurface(struct wl_surface *k) {
  auto iter = m_map_window.find(k);
  if (iter == m_map_window.end()) {
    return nullptr;
  }
  return iter->second;
}

void WaylandDisplay::BindSurface(struct wl_surface *surface,
                                 ISurfacePointerCallback *callback) {
  WaylandDisplayPrivate::getInstance().BindSurface(surface, callback);
}
void WaylandDisplay::UnbindSurface(struct wl_surface *surface) {
  WaylandDisplayPrivate::getInstance().UnbindSurface(surface);
}

} // namespace ui