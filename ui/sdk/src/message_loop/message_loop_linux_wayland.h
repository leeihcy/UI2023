#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_LINUX_WAYLAND_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_LINUX_WAYLAND_H_

#include "include/macro/uidefine.h"
#include "message_loop.h"
#include "src/window/linux/display_wayland.h"


namespace ui {

class MessageLoopPlatformLinuxWayland : public MessageLoopPlatform {
public:
  void Initialize(MessageLoop*) override;
  void Release() override;

  void Run() override;
  void Quit() override;

  void PostTask(PostTaskType &&task) override;
  int  ScheduleTask(ScheduleTaskType &&task, int delay_ms) override;

  TimerID CreateTimer(int interval) override;
  void DestroyTimer(TimerID timer_fd) override;

  void CreateAnimateTimer(int fps) override;
  void DestroyAnimateTimer() override;

private:
  int create_timer(long long interval_ns);
  void process_uithread_pipe_messasge();

private:
  MessageLoop* m_message_loop = nullptr;
  bool m_running = true;

  // epoll
  int m_epoll_fd = -1;
  // 定时器
  int m_animate_timer_fd = -1;
  
  WaylandDisplay m_display;
};

} // namespace ui
#endif