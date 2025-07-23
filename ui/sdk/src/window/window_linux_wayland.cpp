#include "window_linux_wayland.h"
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "include/interface.h"
#include "include/interface/iwindow.h"
#include "include/macro/uidefine.h"
#include "src/graphics/skia/skia_render.h"
#include "src/window/linux/xdg-shell-client-protocol.h"

namespace ui {

WindowPlatformLinuxWayland::WindowPlatformLinuxWayland(ui::Window &w)
    : m_ui_window(w) {}
WindowPlatformLinuxWayland::~WindowPlatformLinuxWayland() {
  Destroy();
}

void WindowPlatformLinuxWayland::Initialize() {}
void WindowPlatformLinuxWayland::Release() {}

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

bool WindowPlatformLinuxWayland::Create(CreateWindowParam& param) {
  if (param.position) {
    m_width = param.w;
    m_height = param.h;
  } else {
    // TBD: 
    m_width = 400;
    m_height = 400;
  }

  create_surface();

  // 显示后再创建top level
  return true;
}

void WindowPlatformLinuxWayland::destroy_surface() {

  if (m_xdg_surface) {
    m_display.UnbindSurface(m_surface);

    xdg_surface_destroy(m_xdg_surface);
    m_xdg_surface = nullptr;
  }
  if (m_surface) {
    wl_surface_destroy(m_surface);
    m_surface = nullptr;
  }
}
void WindowPlatformLinuxWayland::create_surface() {
  m_surface = wl_compositor_create_surface(m_display.get_wl_compositor());
  m_xdg_surface =
      xdg_wm_base_get_xdg_surface(m_display.get_xdg_wm_base(), m_surface);
  xdg_surface_add_listener(m_xdg_surface, &xdg_surface_listener, this);

  m_display.BindSurface(m_surface, this);
}

void WindowPlatformLinuxWayland::create_toplevel() {

  m_xdg_toplevel = xdg_surface_get_toplevel(m_xdg_surface);
  xdg_toplevel_add_listener(m_xdg_toplevel, &xdg_toplevel_listener, this);

  xdg_toplevel_set_title(m_xdg_toplevel, m_title_utf8.c_str());

  // 将app id默认设置为程序名称
  char path[PATH_MAX] = {0};
  ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
  if (len != -1) {
    path[len] = '\0';
    xdg_toplevel_set_app_id(m_xdg_toplevel, basename(path));
  }
  if (m_visible >= WaylandVisibleState::Visible) {
    wl_surface_commit(m_surface);
  }
}
void WindowPlatformLinuxWayland::destroy_toplevel() {
  if (m_xdg_toplevel) {
    xdg_toplevel_destroy(m_xdg_toplevel);
    m_xdg_toplevel = nullptr;
  }
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
void WindowPlatformLinuxWayland::Show() {
  if (m_visible >= WaylandVisibleState::Visible) {
    return;
  }
  WaylandVisibleState old = m_visible;
  m_visible = WaylandVisibleState::Visible;
  on_visible_state_changed(old);
}

void WindowPlatformLinuxWayland::on_visible_state_changed(
    WaylandVisibleState old) {

  // wayland没有支持显示和隐藏。这里通过创建和销毁toplevel来实现显隐
  if (m_visible >= WaylandVisibleState::Visible) {
    create_toplevel();
  } else if (m_visible == WaylandVisibleState::Hidden) {
    destroy_toplevel();
  } else if (m_visible == WaylandVisibleState::Minimized) {
    // TODO:
  }
}

void WindowPlatformLinuxWayland::GetClientRect(Rect *prect) {
  prect->Set(0, 0, m_width, m_height);
}
void WindowPlatformLinuxWayland::GetWindowRect(Rect *prect) {
  prect->Set(0, 0, m_width, m_height);
}
void WindowPlatformLinuxWayland::SetWindowPos(int x, int y, int w, int h,
                                              SetPositionFlags flags) {
  if (flags.size) {
    m_width = w;
    m_height = h;
  }
  if (m_xdg_toplevel && m_xdg_surface) {
    xdg_surface_set_window_geometry(m_xdg_surface, 0, 0, m_width, m_height);
  }
}

// void WindowPlatformLinuxWayland::Invalidate(const Rect *prect) {
//   m_ui_window.onPaint(prect);
// }

bool WindowPlatformLinuxWayland::IsChildWindow() { return false; }
bool WindowPlatformLinuxWayland::IsWindowVisible() { return m_visible >= WaylandVisibleState::Visible; }
void WindowPlatformLinuxWayland::Commit(IRenderTarget *pRT, const Rect *prect,
                                        int count) {
  if (m_shm.data() == nullptr) {
    return;
  }

  if (pRT->GetGraphicsRenderLibraryType() !=
      GRAPHICS_RENDER_LIBRARY_TYPE_SKIA) {
    return;
  }
  SkiaRenderTarget *skiaRT = static_cast<SkiaRenderTarget *>(pRT);
  SkSurface *surface = skiaRT->GetSkiaSurface();
  if (!surface) {
    return;
  }

  SkPixmap pm;
  if (!surface->peekPixels(&pm)) {
    return;
  }

  memcpy(m_shm.data(), pm.addr(), m_shm.size());
  wl_surface_commit(m_surface);

  for (int i = 0; i < count; i++) {
    const Rect &rcItem = prect[i];

    int dst_stride = m_width * 4;
    int src_stride = pm.rowBytes();

    byte *pixelDest =
        (byte *)m_shm.data() + rcItem.top * dst_stride + rcItem.left * 4;
    byte *pixelSrc =
        (byte *)pm.addr() + rcItem.top * src_stride + rcItem.left * 4;

    int height = rcItem.Height();
    for (int y = 0; y < height; y++) {
      memcpy(pixelDest, pixelSrc, rcItem.Width() * 4);
      pixelDest += dst_stride;
      pixelSrc += src_stride;
    }

    wl_surface_damage(m_surface, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height()); 
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
    old_width = m_width;
    old_height = m_height;

    m_width = width;
    m_height = height;
  }

  if (old_width != m_width || old_height != m_height) {
    on_size();
  }

  m_ui_window.onPaint(nullptr); // TODO: 如何监听刷新事件。
}

void WindowPlatformLinuxWayland::on_size() {

  m_shm.Alloc(m_width, m_height);

  struct wl_shm_pool *pool =
      wl_shm_create_pool(m_display.get_wl_shm(), m_shm.fd(), m_shm.size());
  struct wl_buffer *buffer = wl_shm_pool_create_buffer(
      pool, 0, m_width, m_height, m_width * 4, WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);

// #if defined(_DEBUG)
//  memset(m_shm.data(), 0xff, m_shm.size()); // white background
// #endif

  wl_surface_attach(m_surface, buffer, 0, 0);
  m_ui_window.onSize(m_width, m_height);
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
void WindowPlatformLinuxWayland::on_pointer_motion(uint32_t time, wl_fixed_t fixed_x,
                                                   wl_fixed_t fixed_y) {
  int x = wl_fixed_to_int(fixed_x);  
  int y = wl_fixed_to_int(fixed_y); 
  m_ui_window.m_mouse_key.OnMouseMove(x, y);
}
void WindowPlatformLinuxWayland::on_pointer_button(uint32_t serial, uint32_t time,
                                                   uint32_t button,
                                                   uint32_t state, wl_fixed_t fixed_x,
                                                   wl_fixed_t fixed_y) {
  int x = wl_fixed_to_int(fixed_x);  
  int y = wl_fixed_to_int(fixed_y);  

  if (button == BTN_LEFT) {
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
      m_ui_window.m_mouse_key.OnLButtonDown(x, y);

      // TODO: 实现decorate
      if (y < 50) {
        xdg_toplevel_move(m_xdg_toplevel, m_display.get_wl_seat(), serial);
      } 

    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED)  {
      m_ui_window.m_mouse_key.OnLButtonUp(x, y);
    }
  } else if (button == BTN_RIGHT) {
        if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
      m_ui_window.m_mouse_key.OnRButtonDown(x, y);
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED)  {
      m_ui_window.m_mouse_key.OnRButtonUp(x, y);
    }
  }
}

  // IGpuCompositorWindowWayland
struct wl_display* WindowPlatformLinuxWayland::GetWaylandDisplay() {
  return m_display.get_display();
}
struct wl_surface* WindowPlatformLinuxWayland::GetWaylandSurface() {
  return m_surface;
}

} // namespace ui