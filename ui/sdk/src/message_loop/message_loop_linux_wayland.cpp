#include "message_loop_linux_wayland.h"
#include "src/message_loop/message_loop.h"

#include <sys/eventfd.h>
#include <wayland-client.h>
#include <wayland-server-core.h>

namespace ui {

void MessageLoopPlatformLinuxWayland::Initialize(MessageLoop *) {
}
void MessageLoopPlatformLinuxWayland::Release() {}


void MessageLoopPlatformLinuxWayland::Run() {

  struct wl_display *display = m_display.get_display();
  while (m_running && wl_display_dispatch(display) != -1) {
  }
}
void MessageLoopPlatformLinuxWayland::Quit() { m_running = false; }

static void on_async_task(void *data, struct wl_callback *wl_callback,
                          uint32_t callback_data) {
  PostTaskType *p = (PostTaskType *)data;
  p->emit();
  delete p;

  wl_callback_destroy(wl_callback);
}

static const struct wl_callback_listener callback_listener = {
    .done = on_async_task,
};

void MessageLoopPlatformLinuxWayland::PostTask(PostTaskType &&task) {
  PostTaskType *p = new PostTaskType(std::forward<PostTaskType>(task));

  struct wl_callback *cb = wl_display_sync(m_display.get_display());
  wl_callback_add_listener(cb, &callback_listener, p);
  wl_display_flush(m_display.get_display());

  // 这是wayland server的api，这里用不了。
  // wl_event_loop_add_idle(loop, on_idle, p);
}

int MessageLoopPlatformLinuxWayland::ScheduleTask(ScheduleTaskType &&task,
                                                  int delay_ms) {
  return 0;
}
void MessageLoopPlatformLinuxWayland::CreateAnimateTimer(int fps) {}
void MessageLoopPlatformLinuxWayland::DestroyAnimateTimer() {}

} // namespace ui