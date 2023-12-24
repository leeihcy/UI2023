#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_MAC_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_MAC_H_
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../window/linux/display.h"
#include "include/interface/ianimate.h"
#include "message_loop.h"

#include <CoreFoundation/CoreFoundation.h>
#include <cstddef>
#include <memory>

namespace ui {

enum MacMessageLoopType {
    // UI主线程
    _NSAppRun = 0,
    // 其它线程
    _CFRunLoop = 1,
    // 其它线程
    _NSRunLoop = 2,
};

class MessageLoopPlatformMac : public MessageLoopPlatform {
public:
  void Initialize(MessageLoop *) override;
  void Release() override;

  void Run() override;
  void Quit() override;
  void PostTask(PostTaskType &&task) override;
  int ScheduleTask(ScheduleTaskType &&task, int delay_ms) override;
  void CreateAnimateTimer() override;
  void DestroyAnimateTimer() override;

public:
  void onIdle();
  static void onIdleEntry(void *info);
  MessageLoop* GetMessageLoop() { return m_message_loop; }
  void OnAnimateTimer();

private:
  MacMessageLoopType m_type = MacMessageLoopType::_NSAppRun;

  MessageLoop *m_message_loop = nullptr;
  bool quit_flag = false;

  CFRunLoopSourceRef m_idle_source;
  //   CFRunLoopTimerRef m_timer_source;
  CFRunLoopTimerRef m_animate_timer = nullptr;

  signal<void()> m_idle_tasks;
};

} // namespace ui
#endif