#include "display_wayland.h"
#include "src/window/linux/xdg-activation-v1-client-protocol.h"
#include "include/util/log.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <wayland-client-protocol.h>

namespace {
void OnRegistryHandleGlobal(void *data, struct wl_registry *registry,
                               uint32_t id, const char *interface,
                               uint32_t version) {
  ((ui::WaylandDisplayPrivate *)data)
      ->onRegistryHandleGlobal(registry, id, interface, version);
}

void OnXdgWmbasePing(void *data, struct xdg_wm_base *xdg_wm_base,
                             uint32_t serial) {
  xdg_wm_base_pong(xdg_wm_base, serial);
}

}

namespace ui {

WaylandDisplayPrivate &WaylandDisplayPrivate::getInstance() {
  static WaylandDisplayPrivate s;
  return s;
}


static void OnPointerEnter(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t surface_x, wl_fixed_t surface_y) {

  ((WaylandDisplayPrivate *)data)
      ->onPointerEnter(surface, surface_x, surface_y);
}
void WaylandDisplayPrivate::onPointerEnter(struct wl_surface *surface,
                                             wl_fixed_t surface_x,
                                             wl_fixed_t surface_y) {

  ISurfacePointerCallback *callback = MapSurface(surface);
  if (!callback) {
    return;
  }

  m_hover_surface = surface;
  m_last_pointer_x = surface_x;
  m_last_pointer_y = surface_y;

  callback->OnPointerEnter(surface_x, surface_y);
}

static void OnPointerLeave(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface) {
  ((WaylandDisplayPrivate *)data)->onPointerLeave(surface);
}

void WaylandDisplayPrivate::onPointerLeave(struct wl_surface *surface) {
  if (m_hover_surface == surface) {
    m_hover_surface = nullptr;
  }

  ISurfacePointerCallback *callback = MapSurface(surface);
  if (!callback) {
    return;
  }
  callback->OnPointerLeave();
}

static void OnPointerMotion(void *data, struct wl_pointer *pointer,
                           uint32_t time, wl_fixed_t x, wl_fixed_t y) {
  ((WaylandDisplayPrivate *)data)->onPointerMotion(time, x, y);
}

void WaylandDisplayPrivate::onPointerMotion(uint32_t time, wl_fixed_t x,
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
  callback->OnPointerMotion(time, x, y);
}

static void OnPointerButton(void *data, struct wl_pointer *pointer,
                           uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state) {
  ((WaylandDisplayPrivate *)data)
      ->onPointerButton(serial, time, button, state);
}

void WaylandDisplayPrivate::onPointerButton(uint32_t serial, uint32_t time,
                                              uint32_t button, uint32_t state) {
  if (!m_hover_surface) {
    return;
  }

  ISurfacePointerCallback *callback = MapSurface(m_hover_surface);
  if (!callback) {
    return;
  }
  callback->OnPointerButton(serial, time, button, state, m_last_pointer_x,
                              m_last_pointer_y);
}

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

// TODO: 多显示器的场景。
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

  printf("Physical size (mm): %dx%d\n", physical_width, physical_height);
  // 计算 DPI
  float width_inches = (physical_width / 25.4f); // mm → inches
  float height_inches = (physical_height / 25.4f);
  float dpi_horizontal = 2880 / width_inches;
  float dpi_vertical = 1800 / height_inches;
  printf("DPI (horizontal): %.2f\n", dpi_horizontal);
  printf("DPI (vertical): %.2f\n", dpi_vertical);
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

void WaylandDisplayPrivate::on_wl_output_scale(struct wl_output *wl_output,
                                               int32_t factor) {
  m_scale = factor;
}

static void handle_wl_output_scale(void *data, struct wl_output *wl_output,
                                   int32_t factor) {

  ((WaylandDisplayPrivate *)data)->on_wl_output_scale(wl_output, factor);
}


bool WaylandDisplayPrivate::Init() {
  if (m_display) {
    return true;
  }

  // 连接到 Wayland 服务器
  m_display = wl_display_connect(nullptr);
  if (!m_display) {
    UI_LOG_ERROR("[wayland] display connect failed.");
    assert(false);
    return false;
  }

  static const struct wl_registry_listener registry_listener = {
      .global = OnRegistryHandleGlobal,
      .global_remove = nullptr,
  };
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

  initDefaultAppId();

  return true;
}

void WaylandDisplayPrivate::Destroy() {
  // TODO: 
  
  // wl_display_disconnect(m_display);
  // m_display = nullptr;
  // UI_LOG_INFO("wl_display_disconnect");
}

void WaylandDisplayPrivate::initDefaultAppId() {
  // 将app id默认设置为程序名称
  char path[PATH_MAX] = {0};
  ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
  if (len != -1) {
    path[len] = '\0';
    m_default_appid = basename(path);
  } else {
    // todo: random
    m_default_appid = "ui";
  }
}
const char* WaylandDisplayPrivate::GetDefaultAppId() {
  return m_default_appid.c_str();
}

//
// z开头的接口，表示不稳定版本，尽量不使用。
//
void WaylandDisplayPrivate::onRegistryHandleGlobal(
    struct wl_registry *registry, uint32_t id, const char *interface,
    uint32_t version) {
  UI_LOG_DEBUG("[wayland] registry global interface: %s", interface);

  bool bind_xdg_output = false;

  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    m_wl_compositor = (struct wl_compositor *)wl_registry_bind(
        registry, id, &wl_compositor_interface, 4);
  } 
  else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    m_xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(
        registry, id, &xdg_wm_base_interface, 1);

    static const struct xdg_wm_base_listener listener = {
      .ping = OnXdgWmbasePing,
    };
    xdg_wm_base_add_listener(m_xdg_wm_base, &listener, NULL);
  }
  else if (strcmp(interface, wl_shm_interface.name) == 0) {
    m_wl_shm =
        (struct wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
  } 
  else if (strcmp(interface, wl_seat_interface.name) == 0) {
    m_wl_seat =
        (struct wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 1);
    m_wl_pointer = (struct wl_pointer *)wl_seat_get_pointer(m_wl_seat);

    static struct wl_pointer_listener pointer_listener = {
        .enter = &OnPointerEnter,
        .leave = &OnPointerLeave,
        .motion = &OnPointerMotion,
        .button = &OnPointerButton,
    };
    wl_pointer_add_listener(m_wl_pointer, &pointer_listener, this);
  } 
  else if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
    bind_xdg_output = true;
    m_zxdg_output_manager_v1 =
        (struct zxdg_output_manager_v1 *)wl_registry_bind(
            registry, id, &zxdg_output_manager_v1_interface, 1);
  } 
  else if (strcmp(interface, wl_output_interface.name) == 0) {
    bind_xdg_output = true;

    // 关键：版本必须 ≥ 3，否则收不到scale回调。
    m_wl_output = (struct wl_output *)wl_registry_bind(registry, id,
                                                       &wl_output_interface, 3);
    
    static const struct wl_output_listener wl_output_listener_ = {
        .geometry = handle_wl_output_geometry,
        .mode = handle_wl_output_mode,
        .done = handle_wl_output_done,
        // 注意：scale回调必须在wl_output版本≥3时才能收到。
        .scale = handle_wl_output_scale,
    };
    wl_output_add_listener(m_wl_output, &wl_output_listener_, this);
  } 
  else if (strcmp(interface, xdg_activation_v1_interface.name) == 0) {
    m_xdg_activation_v1 = (struct xdg_activation_v1 *)wl_registry_bind(
        registry, id, &xdg_activation_v1_interface, 1);
  } 
  else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0) {
    m_zxdg_decoration_manager_v1 =
        (struct zxdg_decoration_manager_v1 *)wl_registry_bind(
            registry, id, &zxdg_decoration_manager_v1_interface, 1);
  }

  if (bind_xdg_output && m_zxdg_output_manager_v1 && m_wl_output) {
    struct zxdg_output_v1 *xdg_output = zxdg_output_manager_v1_get_xdg_output(
        m_zxdg_output_manager_v1, m_wl_output);
    zxdg_output_v1_add_listener(xdg_output, &xdg_output_listener, NULL);
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

struct wl_seat *WaylandDisplay::get_wl_seat() {
  return WaylandDisplayPrivate::getInstance().m_wl_seat;
}

struct xdg_activation_v1 *WaylandDisplay::get_xdg_activation_v1() {
  return WaylandDisplayPrivate::getInstance().m_xdg_activation_v1;
}
struct zxdg_output_manager_v1 *WaylandDisplay::get_zxdg_output_manager_v1() {
  return WaylandDisplayPrivate::getInstance().m_zxdg_output_manager_v1;
}

struct zxdg_decoration_manager_v1*
WaylandDisplay::get_zxdg_decoration_manager_v1() {
  return WaylandDisplayPrivate::getInstance().m_zxdg_decoration_manager_v1;
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

void WaylandDisplay::GetOutputSize(int *width, int *height) const {
  *width = WaylandDisplayPrivate::getInstance().m_output_width;
  *height = WaylandDisplayPrivate::getInstance().m_output_height;
}
float WaylandDisplay::GetOutputScale() const {
  return WaylandDisplayPrivate::getInstance().m_scale;
}

const char* WaylandDisplay::GetDefaultAppId() {
  return WaylandDisplayPrivate::getInstance().GetDefaultAppId();
}

} // namespace ui