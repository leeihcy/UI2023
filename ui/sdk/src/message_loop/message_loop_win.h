#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_WIN_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_WIN_H_

#include "message_loop.h"

namespace ui {

class MessageLoopPlatformWin : public MessageLoopPlatform {
public:
  MessageLoopPlatformWin();

  void Initialize(MessageLoop*) override;
  void Release() override;

  void Run() override;
  void Quit() override;

  void PostTask(PostTaskType &&task) override;
  int  ScheduleTask(ScheduleTaskType &&task, int delay_ms) override;

private:

private:
  MessageLoop* m_message_loop = nullptr;
  bool quit_flag = false;
};

} // namespace ui
#endif