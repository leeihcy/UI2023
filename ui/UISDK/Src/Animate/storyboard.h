#pragma once

namespace UIA
{
    
interface ITimeline;
interface IStoryboard;
interface IAnimateEventCallback;
class Timeline;
class AnimateManager;
class IdleTimeline;
class FromToTimeline;
class KeyFrameTimeline;
typedef vector<Timeline*>  TimelineList;
typedef vector<Timeline*>::iterator  TimelineIter;
enum E_ANIMATE_TICK_RESULT;

// 用于管理一个动画中的几个timeline 
class Storyboard
{
public:
    Storyboard();
    ~Storyboard();

    bool  AddTimeline(Timeline* p);
    bool  DestroyTimeline(int nTimelineId);
    Timeline*  GetTimeline(unsigned int nIndex);
    Timeline*  FindTimeline(int nTimelineId);
 	IdleTimeline*   CreateIdleTimeline(int nTimelineId);
	FromToTimeline*  CreateTimeline(int nId);
	KeyFrameTimeline*  CreateKeyFrameTimeline(int nId);

    void  SetEventListener(IAnimateEventCallback* pNotify);
    IAnimateEventCallback*  GetEventListener();
	void  SetRef(Storyboard** pp);

    void  SetId(int nID) { m_nID = nID; }
    int   GetId() { return m_nID; }
    void    SetWParam(WPARAM wParam) { m_pWParam = wParam; }
    WPARAM  GetWParam() { return m_pWParam; }
    void    SetLParam(LPARAM lParam) { m_pLParam = lParam; }
    LPARAM  GetLParam() { return m_pLParam; }

    void  Begin();
    void  BeginBlock();
	void  BeginFullCpu();
    void  BeginDelay(long lElapse);

    IStoryboard*  GetIStoryboard();

	E_ANIMATE_TICK_RESULT  NotifyTick();
	void  NotifyStart();
	void  NotifyEnd();
	void  NotifyCancel();
	void  NotifyDiscard();
	void  NotifyReverse();
	void  NotifyRepeat();

	void  MarkToBeDestroyed();

public:
    void  SetAnimateMgr(AnimateManager*  p);
    void  OnAnimateStart();
    void  SetFinish();
    bool  IsFinish();
    bool  OnTick();
    void  Cancel();

    bool  IsDelayWaiting();
    void  ClearDelayWaiting();
    void  UpdateDelayWaiting();

private:
    void  DestroyAllTimeline();

protected:
    AnimateManager*    m_pAnimateMgr;
    IStoryboard*       m_pIStoryboard;
	Storyboard**       m_ppRef;

    IAnimateEventCallback*     m_pNotify;
    int                m_nID;
    WPARAM             m_pWParam;
    LPARAM             m_pLParam;

    unsigned long      m_lDelayBeginTick;  // 延时启用的动画。当GetTickCount大于该值时，启动动画

    TimelineList       m_listTimeline;  // 使用数组，加快外部直接定位，避免查找
};

}