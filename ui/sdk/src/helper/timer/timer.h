#pragma once
#include "include/common/signalslot/signal.h"
#include "include/common/signalslot/slot.h"
#include "include/macro/uidefine.h"
#include "include/interface/ianimate.h"
#include <map>
#include <vector>

namespace ui {
class Application;

// 
// 1. 普通定时器
// 2. 全局共享的一个60fps定时器，例如用于GIF帧轮转
//

class TimerHelper : public uia::IAnimateEventCallback {
public:
  TimerHelper(Application& app);
  ~TimerHelper();

  TimerID SetTimer(int elapse, slot<bool(TimerID)>&& timer_callback);
  void KillTimer(TimerID timer_id);

  void SubscribeAnimateTimer(IAnimateTimer* subscriber);
  void UnsubscribeAnimateTimer(IAnimateTimer* subscriber);

private:
  void onTimer(TimerID timer_id);
  friend class MessageLoop;

  void startAnimateTimer();
  void stopAnimateTimer();
  uia::eAnimateTickResult OnAnimateTick(uia::IStoryboard *) override;
  void OnAnimateEnd(uia::IStoryboard *, uia::eAnimateEndReason e) override;

private:
  Application& m_app;
  std::map<TimerID, signal<bool(TimerID)>> m_mapTimerItem;

  std::vector<IAnimateTimer*> m_animate_timer_subscribers;
  bool m_looping = false;
  uia::IStoryboard* m_storyboard = nullptr;
  
};

} // namespace ui