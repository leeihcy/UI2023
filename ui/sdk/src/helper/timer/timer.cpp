#include "timer.h"
#include "include/interface/ianimate.h"
#include "sdk/src/application/uiapplication.h"
#include <utility>

namespace ui {

TimerHelper::TimerHelper(Application &app) : m_app(app) {}
TimerHelper::~TimerHelper() { stopAnimateTimer(); }

TimerID TimerHelper::SetTimer(int elapse,
                              slot<bool(TimerID)> &&timer_callback) {
  TimerID timer_id = m_app.GetMessageLoop().CreateTimer(elapse);
  m_mapTimerItem[timer_id].connect(
      std::forward<slot<bool(TimerID)>>(timer_callback));
  return timer_id;
}

void TimerHelper::KillTimer(TimerID timer_id) {
  auto iter = m_mapTimerItem.find(timer_id);
  if (iter != m_mapTimerItem.end()) {
    m_app.GetMessageLoop().DestroyTimer(timer_id);
    m_mapTimerItem.erase(iter);
  }
}

void TimerHelper::onTimer(TimerID timer_id) {
  auto iter = m_mapTimerItem.find(timer_id);
  if (iter != m_mapTimerItem.end()) {
    bool continue_timer = iter->second.emit(timer_id);
    if (!continue_timer) {
      KillTimer(timer_id);
    }
  } else {
    KillTimer(timer_id);
  }
}

void TimerHelper::SubscribeAnimateTimer(IAnimateTimer *subscriber) {
  m_animate_timer_subscribers.push_back(subscriber);

  if (m_animate_timer_subscribers.size() == 1) {
    startAnimateTimer();
  }
}

void TimerHelper::UnsubscribeAnimateTimer(IAnimateTimer *subscriber) {
  auto iter = std::find(m_animate_timer_subscribers.begin(),
                        m_animate_timer_subscribers.end(), subscriber);
  if (iter == m_animate_timer_subscribers.end()) {
    return;
  }
  size_t size = m_animate_timer_subscribers.size();
  if (m_looping) {
    *iter = nullptr;
    return;
  }
  
  m_animate_timer_subscribers.erase(iter);
  if (size == 1) {
    stopAnimateTimer();
  }
}

uia::eAnimateTickResult TimerHelper::OnAnimateTick(uia::IStoryboard *) {
  m_looping = true;

  bool dirty = false;
  auto iter = m_animate_timer_subscribers.begin();
  for (; iter != m_animate_timer_subscribers.end();) {
    if (!*iter) {
      dirty = true;
      iter = m_animate_timer_subscribers.erase(iter);
      continue;
    }
    (*iter)->OnTick();
    iter++;
  }
  m_looping = false;

  if (dirty && m_animate_timer_subscribers.empty()) {
    return uia::eAnimateTickResult::Cancel;  
  }
  return uia::eAnimateTickResult::Continue;
}

void TimerHelper::OnAnimateEnd(uia::IStoryboard *, uia::eAnimateEndReason e) {
  m_storyboard = nullptr;
}

void TimerHelper::startAnimateTimer() {
  if (m_storyboard) {
    return;
  }
  m_storyboard = m_app.GetAnimate()->CreateStoryboard(this);
  m_storyboard->CreateIdleTimeline(0);
  m_storyboard->Begin();
}
void TimerHelper::stopAnimateTimer() {
  if (!m_storyboard) {
    return;
  }
  m_app.GetAnimate()->RemoveStoryboard(m_storyboard);
  m_storyboard = nullptr;
}

} // namespace ui