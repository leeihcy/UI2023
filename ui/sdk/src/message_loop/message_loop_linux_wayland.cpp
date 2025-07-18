#include "message_loop_linux_wayland.h"
#include "src/message_loop/message_loop.h"

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <wayland-client.h>
#include <wayland-server-core.h>

namespace ui {

void MessageLoopPlatformLinuxWayland::Initialize(MessageLoop * message_loop) { 
  m_message_loop = message_loop;
  m_epoll_fd = epoll_create1(0);
}
void MessageLoopPlatformLinuxWayland::Release() {}

void MessageLoopPlatformLinuxWayland::Run() {
  struct wl_display *display = m_display.get_display();

  // while (m_running && wl_display_dispatch(display) != -1) {
  // }

  int fd_wayland = wl_display_get_fd(display);

  struct epoll_event ev;
  // 监听 Wayland 事件
  ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
  ev.data.fd = fd_wayland;
  epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd_wayland, &ev);

  while (m_running) {
    struct epoll_event events[2];
    int n = epoll_wait(m_epoll_fd, events, 2, -1); // 阻塞等待事件

    for (int i = 0; i < n; i++) {
      if (events[i].data.fd == fd_wayland) {
        // Wayland 事件就绪，调用 dispatch
        if (wl_display_dispatch(display) == -1) {
          printf("wl_display_dispatch failed");
          m_running = false;
          break;
        }
      }
      else if (events[i].data.fd == m_animate_timer_fd) {
          // 定时器触发
          // 必须读取以清除事件
          uint64_t exp;
          read(m_animate_timer_fd, &exp, sizeof(exp));  
          m_message_loop->OnAnimateTimer();
      }
    }
  }

  close(m_epoll_fd);
  m_epoll_fd = -1;
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
void MessageLoopPlatformLinuxWayland::CreateAnimateTimer(int fps) {
  if (fps == 0) {
    fps = 60;
  }
  if (m_animate_timer_fd > -1) {
    return;
  }
  assert(m_epoll_fd > -1);

  long long period = 1000 * 1000 * 1000 / fps;

  m_animate_timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  struct itimerspec its = {
      .it_interval = {.tv_sec = 0, .tv_nsec =  period},
      .it_value = {.tv_sec = 0, .tv_nsec = period}};
  timerfd_settime(m_animate_timer_fd, 0, &its, NULL);

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = m_animate_timer_fd;
  epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_animate_timer_fd, &ev);
}

void MessageLoopPlatformLinuxWayland::DestroyAnimateTimer() {
  if (m_animate_timer_fd > -1) {
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = m_animate_timer_fd;
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_animate_timer_fd, &ev);

    close(m_animate_timer_fd);
    m_animate_timer_fd = -1;
  }
}

} // namespace ui