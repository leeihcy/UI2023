#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_MAC_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_MAC_H_
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../window/linux/display.h"
#include "glib.h"
#include "message_loop.h"

#include <CoreFoundation/CoreFoundation.h>

namespace ui {

class MessageLoopPlatformMac : public MessageLoopPlatform {
public:
  void Initialize(MessageLoop *) override;
  void Release() override;

  void Run() override;
  void Quit() override;
  void PostTask(PostTaskType &&task) override;
  int ScheduleTask(ScheduleTaskType &&task, int delay_ms) override;

public:
  void onIdle();
  static void onIdleEntry(void *info);

private:
  MessageLoop *m_message_loop = nullptr;
  bool quit_flag = false;

  CFRunLoopSourceRef m_idle_source;
  //   CFRunLoopTimerRef m_timer_source;

  signal<void()> m_idle_tasks;
};

} // namespace ui
#endif