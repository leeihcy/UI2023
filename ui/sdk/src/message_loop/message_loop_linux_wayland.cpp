#include "message_loop_linux_wayland.h"

namespace ui {
void MessageLoopPlatformLinuxWayland::Initialize(MessageLoop *) {
  m_display.Init();
}
void MessageLoopPlatformLinuxWayland::Release() { m_display.Destroy(); }

void MessageLoopPlatformLinuxWayland::Run() {
  struct wl_display *display = m_display.get_display();
  while (!quit_flag && wl_display_dispatch(display) != -1) {
  }
}
void MessageLoopPlatformLinuxWayland::Quit() {
  quit_flag = true;
}

void MessageLoopPlatformLinuxWayland::PostTask(PostTaskType &&task) {}
int MessageLoopPlatformLinuxWayland::ScheduleTask(ScheduleTaskType &&task,
                                                  int delay_ms) {
  return 0;
}
void MessageLoopPlatformLinuxWayland::CreateAnimateTimer(int fps) {}
void MessageLoopPlatformLinuxWayland::DestroyAnimateTimer() {}

} // namespace ui