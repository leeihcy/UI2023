#include "window_linux_wayland.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

namespace ui {

WindowPlatformLinuxWayland::WindowPlatformLinuxWayland(ui::Window &w)
    : m_ui_window(w) {
  m_display.Init();
}
WindowPlatformLinuxWayland::~WindowPlatformLinuxWayland() {
  if (m_xdg_toplevel) {
    xdg_toplevel_destroy(m_xdg_toplevel);
    m_xdg_toplevel = nullptr;
  }
  if (m_xdg_surface) {
    xdg_surface_destroy(m_xdg_surface);
    m_xdg_surface = nullptr;
  }
  if (m_surface) {
    wl_surface_destroy(m_surface);
    m_surface = nullptr;
  }

  m_display.Destroy();
}

void WindowPlatformLinuxWayland::Initialize() {}
void WindowPlatformLinuxWayland::Release() {}

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
  xdg_surface_ack_configure(xdg_surface, serial);
  ((WindowPlatformLinuxWayland *)data)->_on_xdg_surface_configure(xdg_surface);
}
static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

bool WindowPlatformLinuxWayland::Create(const Rect &rect) {

  m_surface = wl_compositor_create_surface(m_display.get_wl_compositor());
  m_xdg_surface =
      xdg_wm_base_get_xdg_surface(m_display.get_xdg_wm_base(), m_surface);
  xdg_surface_add_listener(m_xdg_surface, &xdg_surface_listener, this);
  m_xdg_toplevel = xdg_surface_get_toplevel(m_xdg_surface);

  wl_surface_commit(m_surface);
  return false;
}

WINDOW_HANDLE WindowPlatformLinuxWayland::GetWindowHandle() { return 0; }
void WindowPlatformLinuxWayland::SetTitle(const char *title) {}
void WindowPlatformLinuxWayland::Show() {}
float WindowPlatformLinuxWayland::GetScaleFactor() { return 1.0f; }
void WindowPlatformLinuxWayland::GetClientRect(Rect *prect) {}
void WindowPlatformLinuxWayland::GetWindowRect(Rect *prect) {}
void WindowPlatformLinuxWayland::SetWindowPos(int x, int y, int w, int h,
                                              SetPositionFlags flags) {}
void WindowPlatformLinuxWayland::Invalidate(const Rect *prect) {}
bool WindowPlatformLinuxWayland::IsChildWindow() { return false; }
bool WindowPlatformLinuxWayland::IsWindowVisible() { return true; }
void WindowPlatformLinuxWayland::Commit(IRenderTarget *pRT, const Rect *prect,
                                        int count) {}

// 创建共享内存缓冲区
struct wl_buffer *
WindowPlatformLinuxWayland::create_shm_buffer(int width, int height,
                                              uint32_t format) {
  int stride = width * 4; // 每个像素4字节 (ARGB8888)
  int size = stride * height;

  // 创建临时文件
  char filename[] = "/tmp/wayland-shm-XXXXXX";
  int fd = mkstemp(filename);
  if (fd < 0) {
    fprintf(stderr, "无法创建共享内存文件\n");
    return NULL;
  }
  unlink(filename); // 文件用完即删

  // 调整文件大小
  if (ftruncate(fd, size) < 0) {
    close(fd);
    return NULL;
  }

  // 映射内存
  uint32_t *data =
      (uint32_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    close(fd);
    return NULL;
  }

  // 创建 Wayland 缓冲区
  struct wl_shm_pool *pool =
      wl_shm_create_pool(m_display.get_wl_shm(), fd, size);
  struct wl_buffer *buffer =
      wl_shm_pool_create_buffer(pool, 0, width, height, stride, format);
  wl_shm_pool_destroy(pool);
  close(fd);

  memset(data, 0, width * height * 4);
  // 填充颜色（红色）
  for (int i = 0; i < width * height; i += 16) {
    data[i] = 0xFFFF0000; // ARGB格式: 0xAARRGGBB
  }

  munmap(data, size);

  return buffer;
}

void WindowPlatformLinuxWayland::_on_xdg_surface_configure(
    struct xdg_surface *xdg_surface) {
  // 创建并提交缓冲区（窗口内容）
  struct wl_buffer *buffer =
      create_shm_buffer(640, 480, WL_SHM_FORMAT_ARGB8888);
  wl_surface_attach(m_surface, buffer, 0, 0);
  wl_surface_commit(m_surface);
}

} // namespace ui