#include "timer.h"
#include "sdk/src/application/uiapplication.h"
#include <utility>

namespace ui {

TimerHelper::TimerHelper(Application &app) : m_app(app) {}
#if 0

UINT_PTR TimerHelper::SetTimer(int nElapse, IMessage *pNotify) {
  if (!pNotify)
    return 0;

  TimerItem ti;
  ti.pNotify = pNotify;
  return this->SetTimer(nElapse, &ti);
}

UINT_PTR
TimerHelper::SetTimer(int nElapse,
                      std::function<bool(UINT_PTR, TimerItem *)> func) {
  if (!func)
    return 0;

  TimerItem ti;
  ti.func = func;
  return this->SetTimer(nElapse, &ti);
}

UINT_PTR TimerHelper::SetTimerById(int nElapse, UINT_PTR nId,
                                   IMessage *pNotify) {
  if (nullptr == pNotify)
    return 0;

  // ���Ҹ�pNotify��Ӧ��nId�Ƿ��Ѿ�����
  _MyIter iter = m_mapTimerItem.begin();
  for (; iter != m_mapTimerItem.end(); iter++) {
    if (iter->second.nId == nId && iter->second.pNotify == pNotify)
      return 0;
  }

  TimerItem ti;
  ti.nId = nId;
  ti.pNotify = pNotify;
  return this->SetTimer(nElapse, &ti);
}

UINT_PTR TimerHelper::SetTimer(int nElapse, TimerItem *pItem) {
  if (nullptr == pItem)
    return 0;

  UINT_PTR nTimerID = ::SetTimer(nullptr, 0, nElapse, TimerHelper::TimerProc);
  m_mapTimerItem[nTimerID] = *pItem;

  TimerItem &itemRef = m_mapTimerItem[nTimerID];
  if (itemRef.pNotify)
    itemRef.pNotify->AddDelayRef((void **)&itemRef.pNotify);

  //    int nCount = m_mapTimerItem.size();
  //    UI_LOG_DEBUG("%s  timer id:%d, nElapse=%d, id=%d, pNotify=0x%x",
  //    FUNC_NAME, nTimerID, nElapse, pItem->nId, pItem->pNotify);
  return nTimerID;
}
void TimerHelper::KillTimer(UINT_PTR &nTimerIdRef) {
  _MyIter iter = m_mapTimerItem.find(nTimerIdRef);
  if (iter != m_mapTimerItem.end()) {
    //        UI_LOG_DEBUG("%s timer id:%d", FUNC_NAME, nTimerIdRef);

    if (iter->second.pNotify)
      iter->second.pNotify->RemoveDelayRef((void **)&iter->second.pNotify);

    ::KillTimer(0, nTimerIdRef);

    m_mapTimerItem.erase(iter);
    nTimerIdRef = 0;
    return;
  }
}

void TimerHelper::KillTimerById(int nId, IMessage *pNotify) {
  _MyIter iter = m_mapTimerItem.begin();
  for (; iter != m_mapTimerItem.end(); iter++) {
    if (iter->second.nId == nId && iter->second.pNotify == pNotify) {
      //            UI_LOG_DEBUG("%s timer id:%d, id=%d", FUNC_NAME,
      //            iter->first, nId);
      if (iter->second.pNotify)
        iter->second.pNotify->RemoveDelayRef((void **)&iter->second.pNotify);

      ::KillTimer(nullptr, iter->first);
      m_mapTimerItem.erase(iter);
      return;
    }
  }
}
void TimerHelper::KillTimerByNotify(IMessage *pNotify) {
  _MyIter iter = m_mapTimerItem.begin();
  for (; iter != m_mapTimerItem.end();) {
    if (iter->second.pNotify == pNotify) {
      if (iter->second.pNotify)
        iter->second.pNotify->RemoveDelayRef((void **)&iter->second.pNotify);

      //            UI_LOG_DEBUG("%s timer id:%d, id=%d", FUNC_NAME,
      //            iter->first, nId);
      ::KillTimer(nullptr, iter->first);
      iter = m_mapTimerItem.erase(iter);
    } else {
      iter++;
    }
  }
}

VOID CALLBACK TimerHelper::TimerProc(HWND hwnd, unsigned int uMsg,
                                     UINT_PTR idEvent, unsigned int dwTime) {
  TimerHelper::GetInstance()->OnTimer(idEvent);
}
void TimerHelper::OnTimer(UINT_PTR idEvent) {
  if (0 == m_mapTimerItem.count(idEvent)) {
    UIASSERT(0);
    TimerHelper::GetInstance()->KillTimer(idEvent);
    return;
  }
  TimerItem &ti = m_mapTimerItem[idEvent];

  if (-1 != ti.nRepeatCount)
    ti.nRepeatCount--;

  int nRepeatCount = ti.nRepeatCount;
  if (ti.pNotify) {
    ::UISendMessage(ti.pNotify, WM_TIMER, idEvent, (long)&ti);
  } else if (ti.pProc) {
    ti.pProc(idEvent, &ti);
  } else if (ti.func) {
    if (false == ti.func(idEvent, &ti)) {
      nRepeatCount = 0;
    }
  }

  if (0 == nRepeatCount) {
    KillTimer(idEvent);
  }
}
#endif

unsigned int TimerHelper::SetTimer(int elapse,
                                   slot<bool(unsigned int)> &&timer_callback) {
  int timer_id = m_app.GetMessageLoop().CreateTimer(elapse);
  m_mapTimerItem[timer_id].connect(
      std::forward<slot<bool(unsigned int)>>(timer_callback));
  return timer_id;
}
void TimerHelper::KillTimer(unsigned int timer_id) {
  auto iter = m_mapTimerItem.find(timer_id);
  if (iter != m_mapTimerItem.end()) {
    m_app.GetMessageLoop().DestroyTimer(timer_id);
    m_mapTimerItem.erase(iter);
  }
}
void TimerHelper::onTimer(unsigned int timer_id) {
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

} // namespace ui