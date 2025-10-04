#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_WIN_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_MESSAGE_LOOP_WIN_H_

#include "message_loop.h"
#include "win/msghelper.h"

namespace ui {


class MessageLoopPlatformWin : public MessageLoopPlatform {
public:
  MessageLoopPlatformWin();
  ~MessageLoopPlatformWin();

  void Initialize(MessageLoop*) override;
  void Release() override;

  void Run() override;
  void Quit() override;

  void PostTask(PostTaskType &&task) override;
  int  ScheduleTask(ScheduleTaskType &&task, int delay_ms) override;
  
  TimerID CreateTimer(int interval) override;
  void DestroyTimer(TimerID timeri_id) override;
  void CreateAnimateTimer(int fps) override;
  void DestroyAnimateTimer() override;

private:
  void Run(bool *quit_ref);
	bool IsDialogMessage(::MSG* pMsg);

  // void OnWaitForHandleObjectCallback(int, int);
  void OnAnimateTimer();

public:
  static HWND s_hwnd_forward_postmsg;

private:
  MessageLoop* m_message_loop = nullptr;
  bool quit_flag = false;

  // WaitForHandlesMgr m_WaitForHandlesMgr;
  MessageFilterMgr m_MsgFilterMgr;
  ForwardPostMessageWindow m_WndForwardPostMsg;

	LARGE_INTEGER     m_liPerFreq;     // 用于帧数计算
	HANDLE m_hTimer;
  HMODULE  m_hModuleWinmm;
};

} // namespace ui
#endif