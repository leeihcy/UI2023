#pragma once
#include "sdk/include/common/signalslot/signal.h"
#include "sdk/include/common/signalslot/slot.h"
#include <map>

namespace ui {
class Application;


//
//  计时器辅助类，在时间到达之后，将给该对象发送WM_TIMER消息
//
class TimerHelper {
public:
  typedef unsigned int TimerId;

  TimerHelper(Application& app);

  unsigned int SetTimer(int elapse, slot<bool(unsigned int)>&& timer_callback);
  void KillTimer(unsigned int timer_id);

  // TimerId  SetTimer(int nElapse, IMessage* pNotify);
  // TimerId  SetTimer(int nElapse, std::function<bool(TimerId,TimerItem*)>
  // func);
  // TimerId  SetTimerById(int nElapse, TimerId nId, IMessage* pNotify);
  // TimerId  SetTimer(int nElapse, TimerItem* pItem);
  // void  KillTimer(TimerId& nTimerIdRef);
  // void  KillTimerById(int nId, IMessage* pNotify);
  // void  KillTimerByNotify(IMessage* pNotify);

  // static VOID CALLBACK TimerProc( HWND hwnd, unsigned int uMsg, TimerId
  // idEvent,	unsigned int dwTime ); void  OnTimer(TimerId idEvent);

protected:
  void onTimer(unsigned int timer_id);
  friend class MessageLoop;

private:
  Application& m_app;
  std::map<TimerId, signal<bool(unsigned int)>> m_mapTimerItem;
  
};

} // namespace ui