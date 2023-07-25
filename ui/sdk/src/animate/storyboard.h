#pragma once
#include "include/interface/ianimate.h"

namespace uia
{
    
struct ITimeline;
struct IStoryboard;
struct IAnimateEventCallback;
class Timeline;
class AnimateManager;
class IdleTimeline;
class FromToTimeline;
class KeyFrameTimeline;
typedef std::vector<Timeline*>  TimelineList;
typedef std::vector<Timeline*>::iterator  TimelineIter;

// ���ڹ���һ�������еļ���timeline 
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
    void    SetWParam(long wParam) { m_pWParam = wParam; }
    long  GetWParam() { return m_pWParam; }
    void    SetLParam(long lParam) { m_pLParam = lParam; }
    long  GetLParam() { return m_pLParam; }

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
    long             m_pWParam;
    long             m_pLParam;

    unsigned long      m_lDelayBeginTick;  // ��ʱ���õĶ�������GetTickCount���ڸ�ֵʱ����������

    TimelineList       m_listTimeline;  // ʹ�����飬�ӿ��ⲿֱ�Ӷ�λ���������
};

}