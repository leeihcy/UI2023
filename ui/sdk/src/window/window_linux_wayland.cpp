#include "window_linux_wayland.h"
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

#include "include/interface.h"
#include "include/interface/iwindow.h"
#include "include/macro/uidefine.h"
#include "include/util/rect.h"
#include "src/graphics/skia/skia_render.h"
#include "src/window/linux/xdg-activation-v1-client-protocol.h"
#include "src/window/linux/xdg-shell-client-protocol.h"

namespace ui {

WindowPlatformLinuxWayland::WindowPlatformLinuxWayland(ui::Window &w)
    : m_ui_window(w) {
  m_bound_dip.SetEmpty();
  m_bound_px.SetEmpty();
}
WindowPlatformLinuxWayland::~WindowPlatformLinuxWayland() { Destroy(); }

void WindowPlatformLinuxWayland::Initialize() {}

void WindowPlatformLinuxWayland::Destroy() {
  destroy_toplevel();
  destroy_surface();
}

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
  xdg_surface_ack_configure(xdg_surface, serial);
  ((WindowPlatformLinuxWayland *)data)->on_xdg_surface_configure(xdg_surface);
}
void xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
                            int32_t width, int32_t height,
                            struct wl_array *states) {
  ((WindowPlatformLinuxWayland *)data)
      ->on_xdg_toplevel_configure(xdg_toplevel, width, height, states);
}

void xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel) {
  ((WindowPlatformLinuxWayland *)data)->on_xdg_toplevel_close(xdg_toplevel);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};
static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
};

bool WindowPlatformLinuxWayland::Create(CreateWindowParam &param) {
  float scale = m_display.GetOutputScale();
  m_ui_window.m_dpi.SetSystemDpi(scale);

  ui::Rect rc_bound;
  if (param.position) {
    rc_bound = ui::Rect::MakeXYWH(param.x, param.y, param.w, param.h);
  } else {
    rc_bound = ui::Rect::MakeXYWH(0, 0, 400, 400);
  }

  m_bound_px = rc_bound;
  m_bound_dip = rc_bound;
  if (param.is_pixel_unit) {
    m_bound_dip.Scale(1 / scale);
  } else {
    m_bound_px.Scale(scale);
  }

  create_surface(scale);

  // 只在Show()时创建toplevel
  // create_toplevel();

  // 这时窗口还没显示，因此模拟一个大小变化事件
  m_ui_window.onSize(m_bound_px.Width(), m_bound_px.Height());
  return true;
}

void WindowPlatformLinuxWayland::destroy_surface() {

  if (m_surface) {
    wl_surface_destroy(m_surface);
    m_surface = nullptr;
  }
}
void WindowPlatformLinuxWayland::create_surface(int scale) {
  m_surface = wl_compositor_create_surface(m_display.get_wl_compositor());
  m_display.BindSurface(m_surface, this);

  // 实现窗口大小与buffer缩放的关键调用。
  // 这样就能以dip单位创建窗口，以pixel单位创建buffer
  wl_surface_set_buffer_scale(m_surface, scale);
}

void WindowPlatformLinuxWayland::create_toplevel() {

  // xdg_wm_base@7: error 0: wl_surface@3 already has a role assigned
  wl_surface_attach(m_surface, nullptr, 0, 0);
  wl_surface_commit(m_surface);

  m_xdg_surface =
      xdg_wm_base_get_xdg_surface(m_display.get_xdg_wm_base(), m_surface);
  xdg_surface_add_listener(m_xdg_surface, &xdg_surface_listener, this);

  m_xdg_toplevel = xdg_surface_get_toplevel(m_xdg_surface);
  xdg_toplevel_add_listener(m_xdg_toplevel, &xdg_toplevel_listener, this);

  xdg_toplevel_set_title(m_xdg_toplevel, m_title_utf8.c_str());

  update_decoration();

  // 设置窗口大小
  if (!m_bound_dip.IsEmpty()) {
    xdg_surface_set_window_geometry(m_xdg_surface, m_bound_dip.left,
                                    m_bound_dip.top, m_bound_dip.Width(),
                                    m_bound_dip.Height());
    on_size();
  }

  // 将app id默认设置为程序名称
  char path[PATH_MAX] = {0};
  ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
  if (len != -1) {
    path[len] = '\0';
    xdg_toplevel_set_app_id(m_xdg_toplevel, basename(path));
  }
}

void WindowPlatformLinuxWayland::destroy_toplevel() {
  if (m_xdg_toplevel) {
    xdg_toplevel_destroy(m_xdg_toplevel);
    m_xdg_toplevel = nullptr;
  }
  if (m_xdg_surface) {
    m_display.UnbindSurface(m_surface);
    xdg_surface_destroy(m_xdg_surface);
    m_xdg_surface = nullptr;
  }
}

void zxdg_toplevel_decoration_configure(
    void *data, struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1,
    uint32_t mode) {
  printf("zxdg_toplevel_decoration_v1 configure mode: %d\n", mode);
}
static constexpr zxdg_toplevel_decoration_v1_listener
    kToplevelDecorationListener = {
        .configure = &zxdg_toplevel_decoration_configure,
};

void WindowPlatformLinuxWayland::update_decoration() {
  if (!m_xdg_toplevel) {
    return;
  }

  auto *decoration_manager = m_display.get_zxdg_decoration_manager_v1();
  if (!decoration_manager) {
    return;
  }

  m_decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(
      m_display.get_zxdg_decoration_manager_v1(), m_xdg_toplevel);
  if (!m_decoration) {
    return;
  }

  zxdg_toplevel_decoration_v1_add_listener(m_decoration,
                                           &kToplevelDecorationListener, this);

  if (m_use_native_frame) {
    zxdg_toplevel_decoration_v1_set_mode(
        m_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
  } else {
    zxdg_toplevel_decoration_v1_set_mode(
        m_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
  }

#if 0

  zxdg_toplevel_decoration_v1_set_mode(zxdg_toplevel_decoration_.get(),
                                       ToInt32(requested_mode));

    if (use_native_frame) {
      xdg_toplevel_set_decorated(m_xdg_toplevel, 1);
    } else {
      xdg_toplevel_set_decorated(m_xdg_toplevel, 0);
    }
#endif
}

WINDOW_HANDLE WindowPlatformLinuxWayland::GetWindowHandle() {
  return (WINDOW_HANDLE)m_surface;
}
void WindowPlatformLinuxWayland::SetTitle(const char *title) {
  if (title) {
    m_title_utf8 = title;
  } else {
    m_title_utf8.clear();
  }
  if (m_xdg_toplevel) {
    xdg_toplevel_set_title(m_xdg_toplevel, m_title_utf8.c_str());
  }
}

//
// wayland没有显示隐藏接口，处理方案：
// 1. 清除窗口内容：memset(m_shm.data(), 0, m_shm.size());
//    问题：只是窗口看不见，但仍然有鼠标事件
// 2. 清空缓存： wl_surface_attach(m_surface, nullptr, 0, 0);
//    问题：仍然有任务栏图标；
//         会报错：wl_surface@3 already has a role assigned
//         m_xdg_surface不能复用，只能重新创建。
// 3. 销毁xdg_surface + toplevel，chromium的做法
//
void WindowPlatformLinuxWayland::Show(bool active) {
  if (m_visible >= WaylandVisibleState::Visible) {
    return;
  }
  WaylandVisibleState old = m_visible;
  m_visible = WaylandVisibleState::Visible;
  on_visible_state_changed(old);

  create_toplevel();

  m_ui_window.onPaint(&m_bound_dip);

  // 需要强制发送请求到合成器，否则不会立即显示。
  wl_display_flush(m_display.get_display());

  // 激活窗口需要放在flush之后，
  if (active) {
    Activate();
  }
}

void WindowPlatformLinuxWayland::Hide() {
  if (m_visible < WaylandVisibleState::Visible) {
    return;
  }
  WaylandVisibleState old = m_visible;
  m_visible = WaylandVisibleState::Hidden;
  on_visible_state_changed(old);

  destroy_toplevel();

  // #if 0
  //   // 清除窗口内容
  //   if (m_shm.data()) {
  //     memset(m_shm.data(), 0, m_shm.size());
  //     wl_surface_commit(m_surface);
  //     // 标记整个表面失效
  //     wl_surface_damage(m_surface, 0, 0, INT32_MAX, INT32_MAX);
  //   }
  // #else
  //   wl_surface_attach(m_surface, nullptr, 0, 0);
  // #endif

  //   wl_surface_commit(m_surface);

  // 需要强制发送请求到合成器，否则不会立即隐藏。
  wl_display_flush(m_display.get_display());
}

struct ActivationData {
  struct xdg_activation_v1 *activation;
  struct xdg_activation_token_v1 *token;
  struct wl_surface *surface;
};

void xdg_activation_token_v1_listener_done(
    void *data, struct xdg_activation_token_v1 *xdg_activation_token_v1,
    const char *token) {
  ActivationData *activation_data = (ActivationData *)data;

  xdg_activation_v1_activate(activation_data->activation, token,
                             activation_data->surface);
  xdg_activation_token_v1_destroy(activation_data->token);

  delete activation_data;
  activation_data = nullptr;
}

const static xdg_activation_token_v1_listener
    xdg_activation_token_v1_listener_ = {
        .done = xdg_activation_token_v1_listener_done};

void WindowPlatformLinuxWayland::Activate() {
  struct xdg_activation_v1 *activation = m_display.get_xdg_activation_v1();
  if (!activation) {
    return;
  }
  struct xdg_activation_token_v1 *token =
      xdg_activation_v1_get_activation_token(activation);
  if (!token) {
    return;
  }

  ActivationData *data = new ActivationData;
  data->activation = activation;
  data->token = token;
  data->surface = m_surface;

  xdg_activation_token_v1_add_listener(
      token, &xdg_activation_token_v1_listener_, data);

  xdg_activation_token_v1_set_surface(token, m_surface);
  xdg_activation_token_v1_commit(token);
}

void WindowPlatformLinuxWayland::on_visible_state_changed(
    WaylandVisibleState old) {}

void WindowPlatformLinuxWayland::GetClientRect(Rect *prect) {
  prect->Set(0, 0, m_bound_px.Width(), m_bound_px.Height());
}
void WindowPlatformLinuxWayland::GetWindowRect(Rect *prect) {
  *prect = m_bound_px;
}
void WindowPlatformLinuxWayland::UpdateNonClientRegion(Rect *region) {}
void WindowPlatformLinuxWayland::GetMonitorWorkArea(Rect *rect) {
  // TODO: 这里没有计算任务栏
  int width = 0;
  int height = 0;
  m_display.GetOutputSize(&width, &height);
  rect->Set(0, 0, width, height);
}

// xywh 乘以了缩放系数，是像素坐标
void WindowPlatformLinuxWayland::SetWindowPos(int x, int y, int w, int h,
                                              SetPositionFlags flags) {
  float scale = m_ui_window.m_dpi.GetScaleFactor();

  if (flags.move) {
    m_bound_px.left = x;
    m_bound_px.top = y;

    m_bound_dip.left = x / scale;
    m_bound_dip.top = y / scale;
  }
  if (flags.size) {

    m_bound_px.right = m_bound_px.left + w;
    m_bound_px.bottom = m_bound_px.top + h;

    m_bound_dip.right = m_bound_dip.left + (w / scale);
    m_bound_dip.bottom = m_bound_dip.top + (h / scale);
  }
  if (m_xdg_surface) {
    xdg_surface_set_window_geometry(m_xdg_surface, m_bound_dip.left,
                                    m_bound_dip.top, m_bound_dip.Width(),
                                    m_bound_dip.Height());
  }
  on_size();
}

// void WindowPlatformLinuxWayland::Invalidate(const Rect *prect) {
//   m_ui_window.onPaint(prect);
// }

bool WindowPlatformLinuxWayland::IsChildWindow() { return false; }
bool WindowPlatformLinuxWayland::IsWindowVisible() {
  return m_visible >= WaylandVisibleState::Visible;
}

void WindowPlatformLinuxWayland::Commit2(const FrameBuffer& fb, const RectRegion &dirty_region_px) {

  if (m_shm.data() == nullptr) {
    return;
  }
  // memcpy(m_shm.data(), pm.addr(), m_shm.size());
  // wl_surface_commit(m_surface);

  for (int i = 0; i < dirty_region_px.Count(); i++) {
    const Rect& rcItem = *dirty_region_px[i];
    
    int dst_stride = m_bound_px.Width() * 4;
    int src_stride = fb.rowbytes;

    byte *pixelDest =
        (byte *)m_shm.data() + rcItem.top * dst_stride + rcItem.left * 4;
    byte *pixelSrc =
        (byte *)fb.data + rcItem.top * src_stride + rcItem.left * 4;

    int height = rcItem.Height();
    for (int y = 0; y < height; y++) {
      memcpy(pixelDest, pixelSrc, rcItem.Width() * 4);
      pixelDest += dst_stride;
      pixelSrc += src_stride;
    }

    wl_surface_damage(m_surface, rcItem.left, rcItem.top, rcItem.Width(),
                      rcItem.Height());
  }

  if (m_visible >= WaylandVisibleState::Visible) {
    wl_surface_commit(m_surface);
  }
}

WaylandSurfaceSharedMemory::WaylandSurfaceSharedMemory() {}
WaylandSurfaceSharedMemory::~WaylandSurfaceSharedMemory() { Free(); }

bool WaylandSurfaceSharedMemory::Alloc(int width, int height) {
  int size = width * height * 4; // 每个像素4字节 (ARGB8888)
  if (m_size == size) {
    return true;
  }
  Free();

  // 创建临时文件
  char filename[] = "/tmp/wayland-shm-XXXXXX";
  int fd = mkstemp(filename);
  if (fd < 0) {
    fprintf(stderr, "无法创建共享内存文件\n");
    return false;
  }
  unlink(filename); // 文件用完即删

  // 调整文件大小
  if (ftruncate(fd, size) < 0) {
    close(fd);
    return false;
  }

  // 映射内存
  uint32_t *data =
      (uint32_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    close(fd);
    return false;
  }

  m_shm_fd = fd;
  m_shm_data = data;

  m_size = size;
  return true;
}

void WaylandSurfaceSharedMemory::Free() {
  if (m_shm_data) {
    munmap(m_shm_data, m_size);
    m_shm_data = nullptr;
  }
  if (m_shm_fd >= 0) {
    close(m_shm_fd);
    m_shm_fd = -1;
  }
  m_size = 0;
}

void WindowPlatformLinuxWayland::on_xdg_surface_configure(
    struct xdg_surface *xdg_surface) {}

// 窗口激活/失活也会触发这个函数
void WindowPlatformLinuxWayland::on_xdg_toplevel_configure(
    struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
    struct wl_array *states) {

  int old_width = 0;
  int old_height = 0;

  if (width == 0 || height == 0) {
    // 使用创建时外部指定的大小。
  } else {
    old_width = m_bound_px.Width();
    old_height = m_bound_px.Height();

    m_bound_dip.right = m_bound_dip.left + width;
    m_bound_dip.bottom = m_bound_dip.top + height;

    m_bound_px = m_bound_dip;
    m_ui_window.m_dpi.ScaleRect(&m_bound_px);
  }

  if (old_width != m_bound_px.Width() || old_height != m_bound_px.Height()) {
    on_size();
  }

  m_ui_window.onPaint(&m_bound_dip);
}

void WindowPlatformLinuxWayland::on_size() {

  m_shm.Alloc(m_bound_px.Width(), m_bound_px.Height());

  struct wl_shm_pool *pool =
      wl_shm_create_pool(m_display.get_wl_shm(), m_shm.fd(), m_shm.size());
  struct wl_buffer *buffer = wl_shm_pool_create_buffer(
      pool, 0, m_bound_px.Width(), m_bound_px.Height(), m_bound_px.Width() * 4,
      WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);

  // #if defined(_DEBUG)
  //  memset(m_shm.data(), 0xff, m_shm.size()); // white background
  // #endif

  wl_surface_attach(m_surface, buffer, 0, 0);
  m_ui_window.onSize(m_bound_px.Width(), m_bound_px.Height());
}

void WindowPlatformLinuxWayland::on_xdg_toplevel_close(
    struct xdg_toplevel *xdg_toplevel) {
  m_ui_window.onClose();
  Destroy();
  m_ui_window.onDestroy();
}

void WindowPlatformLinuxWayland::on_pointer_enter(wl_fixed_t surface_x,
                                                  wl_fixed_t surface_y) {
  m_ui_window.m_mouse_key.OnMouseEnter();

  int x = wl_fixed_to_int(surface_x);
  int y = wl_fixed_to_int(surface_y);
  m_ui_window.m_mouse_key.OnMouseMove(x, y);
}
void WindowPlatformLinuxWayland::on_pointer_leave() {
  m_ui_window.m_mouse_key.OnMouseLeave();
}
void WindowPlatformLinuxWayland::on_pointer_motion(uint32_t time,
                                                   wl_fixed_t fixed_x,
                                                   wl_fixed_t fixed_y) {
  int x = wl_fixed_to_int(fixed_x);
  int y = wl_fixed_to_int(fixed_y);
  m_ui_window.m_mouse_key.OnMouseMove(x, y);
}
void WindowPlatformLinuxWayland::on_pointer_button(
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state,
    wl_fixed_t fixed_x, wl_fixed_t fixed_y) {
  int x = wl_fixed_to_int(fixed_x);
  int y = wl_fixed_to_int(fixed_y);

  if (button == BTN_LEFT) {
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
      m_ui_window.m_mouse_key.OnLButtonDown(x, y);

      // TODO: 实现decorate
      if (y < 50) {
        xdg_toplevel_move(m_xdg_toplevel, m_display.get_wl_seat(), serial);
      }

    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
      m_ui_window.m_mouse_key.OnLButtonUp(x, y);
    }
  } else if (button == BTN_RIGHT) {
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
      m_ui_window.m_mouse_key.OnRButtonDown(x, y);
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
      m_ui_window.m_mouse_key.OnRButtonUp(x, y);
    }
  }
}

// IGpuCompositorWindowWayland
struct wl_display *WindowPlatformLinuxWayland::GetWaylandDisplay() {
  return m_display.get_display();
}
struct wl_surface *WindowPlatformLinuxWayland::GetWaylandSurface() {
  return m_surface;
}

} // namespace ui