#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_LINUX_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_LINUX_H_
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "glib.h"
#include "message_loop.h"
#include "../window/linux/display_x11.h"


namespace ui {

class MessageLoopPlatformLinux : public MessageLoopPlatform {
public:
  void Initialize(MessageLoop*) override;
  void Release() override;

  void Run() override;
  void Quit() override;

  void PostTask(PostTaskType &&task) override;
  int  ScheduleTask(ScheduleTaskType &&task, int delay_ms) override;
  void CreateAnimateTimer(int fps) override;
  void DestroyAnimateTimer() override;
  
public:
  void processXEvent();

private:
  void processXEvent(const XEvent& event);

private:
  MessageLoop* m_message_loop = nullptr;
  bool quit_flag = false;

  GMainLoop *loop = nullptr;
  GMainContext *context = nullptr;

  GSource *m_xevent_source = nullptr;
public:
  X11Display  m_display;
};

} // namespace ui
#endif