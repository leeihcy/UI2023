#include "display_wayland.h"
#include "src/window/linux/xdg-activation-v1-client-protocol.h"

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
  ((WaylandDisplayPrivate *)data)
      ->on_pointer_button(serial, time, button, state);
}

void WaylandDisplayPrivate::on_pointer_button(uint32_t serial, uint32_t time,
                                              uint32_t button, uint32_t state) {
  if (!m_hover_surface) {
    return;
  }

  ISurfacePointerCallback *callback = MapSurface(m_hover_surface);
  if (!callback) {
    return;
  }
  callback->on_pointer_button(serial, time, button, state, m_last_pointer_x,
                              m_last_pointer_y);
}

static struct wl_pointer_listener pointer_listener = {
    .enter = &pointer_enter,
    .leave = &pointer_leave,
    .motion = &pointer_motion,
    .button = &pointer_button,
};

#if 0
// 输出信息回调
static void handle_xdg_output_logical_position(
    void *data, struct zxdg_output_v1 *zxdg_output_v1, int32_t x, int32_t y) {
  printf("Logical position: (%d, %d)\n", x, y);
}

static void
handle_xdg_output_logical_size(void *data,
                               struct zxdg_output_v1 *zxdg_output_v1,
                               int32_t width, int32_t height) {
  printf("Logical size: %dx%d\n", width, height);
}

static void handle_xdg_output_name(void *data,
                                   struct zxdg_output_v1 *zxdg_output_v1,
                                   const char *name) {
  printf("Output name: %s\n", name);
}

static void handle_xdg_output_done(void *data,
                                   struct zxdg_output_v1 *zxdg_output_v1) {
  printf("Output info received.\n");
}

// 绑定 xdg_output 监听器
static const struct zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = handle_xdg_output_logical_position,
    .logical_size = handle_xdg_output_logical_size,
    .done = handle_xdg_output_done,
    .name = handle_xdg_output_name,
};
#endif

static void handle_wl_output_geometry(void *data, struct wl_output *wl_output,
                                      int32_t x, int32_t y,
                                      int32_t physical_width,
                                      int32_t physical_height, int32_t subpixel,
                                      const char *make, const char *model,
                                      int32_t transform) {
  printf(
      "Output geometry: (%d, %d), physical size: %dx%d, make: %s, model: %s\n",
      x, y, physical_width, physical_height, make ? make : "unknown",
      model ? model : "unknown");
}
static void handle_wl_output_mode(void *data, struct wl_output *wl_output,
                                  uint32_t flags, int32_t width, int32_t height,
                                  int32_t refresh) {
  if (flags & WL_OUTPUT_MODE_CURRENT) {
    printf("Physical size: %dx%d\n", width, height);
  }
  ((WaylandDisplayPrivate *)data)
      ->on_wl_output_mode(wl_output, flags, width, height, refresh);
}

void WaylandDisplayPrivate::on_wl_output_mode(struct wl_output *wl_output,
                                              uint32_t flags, int32_t width,
                                              int32_t height, int32_t refresh) {
  m_output_width = width;
  m_output_height = height;
}

static void handle_wl_output_done(void *data, struct wl_output *wl_output) {
  printf("Output done.\n");
}

static void handle_wl_output_scale(void *data, struct wl_output *wl_output,
                                   int32_t factor) {
  printf("Output scale factor: %d\n", factor);
}

static const struct wl_output_listener wl_output_listener_ = {
    .geometry = handle_wl_output_geometry,
    .mode = handle_wl_output_mode,
    .done = handle_wl_output_done,
    .scale = handle_wl_output_scale,
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

  // 第一次同步执行on_registry_handle_global
  wl_display_roundtrip(m_display);
  // 第二次同步执行wl_output等事件
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

  bool bind_xdg_output = false;

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
#if 0
  } else if (strcmp(interface, "zxdg_output_manager_v1") == 0) {
    bind_xdg_output = true;
    m_zxdg_output_manager_v1 =
        (struct zxdg_output_manager_v1 *)wl_registry_bind(
            registry, id, &zxdg_output_manager_v1_interface, 1);
#endif
  } else if (strcmp(interface, "wl_output") == 0) {
    bind_xdg_output = true;
    m_wl_output = (struct wl_output *)wl_registry_bind(registry, id,
                                                       &wl_output_interface, 1);
    wl_output_add_listener(m_wl_output, &wl_output_listener_, this);
  } else if (strcmp(interface, "xdg_activation_v1") == 0) {
    m_xdg_activation_v1 = (struct xdg_activation_v1 *)wl_registry_bind(
        registry, id, &xdg_activation_v1_interface, 1);
  }
#if 0
  if (bind_xdg_output && m_zxdg_output_manager_v1 && m_wl_output) {
    struct zxdg_output_v1 *xdg_output = zxdg_output_manager_v1_get_xdg_output(
        m_zxdg_output_manager_v1, m_wl_output);
    zxdg_output_v1_add_listener(xdg_output, &xdg_output_listener, NULL);
  }
#endif
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

struct wl_seat *WaylandDisplay::get_wl_seat() {
  return WaylandDisplayPrivate::getInstance().m_wl_seat;
}

struct xdg_activation_v1* WaylandDisplay::get_xdg_activation_v1() {
    return WaylandDisplayPrivate::getInstance().m_xdg_activation_v1;
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

void WaylandDisplay::GetOutputSize(int* width, int* height) const {
  *width = WaylandDisplayPrivate::getInstance().m_output_width;
  *height = WaylandDisplayPrivate::getInstance().m_output_height;
}

} // namespace ui