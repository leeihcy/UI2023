#pragma once
#include "include/interface/ianimate.h"
#include <vector>

namespace uia {

struct ITimeline;
struct IStoryboard;
struct IAnimateEventCallback;
class Timeline;
class Animate;
class IdleTimeline;
class FromToTimeline;
class KeyFrameTimeline;
typedef std::vector<Timeline *> TimelineList;
typedef std::vector<Timeline *>::iterator TimelineIter;

// 用于管理一个动画中的几个timeline
class Storyboard {
public:
  Storyboard();
  ~Storyboard();

  bool AddTimeline(Timeline *p);
  bool DestroyTimeline(int nTimelineId);
  Timeline *GetTimeline(unsigned int nIndex);
  Timeline *FindTimeline(int nTimelineId);
  IdleTimeline *CreateIdleTimeline(int nTimelineId);
  FromToTimeline *CreateTimeline(int nId);
  KeyFrameTimeline *CreateKeyFrameTimeline(int nId);

  void SetEventListener(IAnimateEventCallback *pNotify);
  IAnimateEventCallback *GetEventListener();
  void SetRef(Storyboard **pp);

  void SetId(int nID) { m_nID = nID; }
  int GetId() { return m_nID; }
  void SetWParam(ui::llong wParam) { m_pWParam = wParam; }
  ui::llong GetWParam() { return m_pWParam; }
  void SetLParam(ui::llong lParam) { m_pLParam = lParam; }
  ui::llong GetLParam() { return m_pLParam; }

  void Begin();
  void BeginBlock();
  void BeginFullCpu();
  void BeginDelay(long lElapse);

  IStoryboard *GetIStoryboard();

  eAnimateTickResult NotifyTick();
  void NotifyStart();
  void NotifyEnd();
  void NotifyCancel();
  void NotifyDiscard();
  void NotifyReverse();
  void NotifyRepeat();

  void MarkToBeDestroyed();

public:
  void SetAnimateMgr(Animate *p);
  void OnAnimateStart();
  void SetFinish();
  bool IsFinish();
  bool OnTick();
  void Cancel();

  bool IsDelayWaiting();
  void ClearDelayWaiting();
  void UpdateDelayWaiting();

private:
  void DestroyAllTimeline();

protected:
  Animate *m_pAnimateMgr;
  IStoryboard *m_pIStoryboard;
  Storyboard **m_ppRef;

  IAnimateEventCallback *m_pNotify;
  int m_nID;
  ui::llong m_pWParam;
  ui::llong m_pLParam;

  unsigned long
      m_lDelayBeginTick; // 延时启用的动画。当GetTickCount大于该值时，启动动画

  TimelineList m_listTimeline; // 使用数组，加快外部直接定位，避免查找
};

} // namespace uia