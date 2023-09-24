#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_WIN_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_WIN_H_

#include "message_loop.h"
#include "win/msghelper.h"

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
  void Run(bool *quit_ref);
	bool  IsDialogMessage(::MSG* pMsg);
private:
  MessageLoop* m_message_loop = nullptr;
  
  WaitForHandlesMgr m_WaitForHandlesMgr;
  MessageFilterMgr m_MsgFilterMgr;
  ForwardPostMessageWindow m_WndForwardPostMsg;

  bool quit_flag = false;
};

} // namespace ui
#endif