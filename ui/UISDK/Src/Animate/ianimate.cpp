#include "stdafx.h"
#include "animatemgr.h"
#include "storyboard.h"
#include "timeline.h"

namespace UIA
{
// UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindowAnimateBase, WindowAnimateBase)
// UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindowAnimateBase, WindowAnimateBase, Storyboard)
// 
// UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindow3DAnimate, Window3DAnimate)
// UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindow3DAnimate, Window3DAnimate, WindowAnimateBase)
// 
// UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate)
// UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate, WindowAnimateBase)
// 
// UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindowAlphaShowAnimate, WindowAlphaShowAnimate)
// UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindowAlphaShowAnimate, WindowAlphaShowAnimate, WindowAnimateBase)
// 
// UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IControlAnimateBase, ControlAnimateBase)
// UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IControlAnimateBase, ControlAnimateBase, Storyboard)
// 
// UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(ISlideAnimate, SlideAnimate)
// UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(ISlideAnimate, SlideAnimate, ControlAnimateBase)

ITimeline::ITimeline(Timeline* p) : m_pTimelineImpl(p)
{
}

void  ITimeline::SetRepeateCount(int n)
{
    m_pTimelineImpl->SetRepeateCount(n);
}
void  ITimeline::SetTimeType(TIMELINE_TIME_TYPE eType)
{   
    m_pTimelineImpl->SetTimeType(eType);
}
void  ITimeline::SetAutoReverse(bool b)
{
    m_pTimelineImpl->SetAutoReverse(b);
}
bool  ITimeline::IsFinish()
{
    return m_pTimelineImpl->IsFinish();
}
void  ITimeline::SetFinish()
{
    m_pTimelineImpl->SetFinish();
}
float ITimeline::GetCurrentValue()
{
    return m_pTimelineImpl->GetCurrentValue();
}

int   ITimeline::GetCurrentIntValue()
{
	return _round(m_pTimelineImpl->GetCurrentValue());
}
void  ITimeline::SetId(int nID)
{
    m_pTimelineImpl->SetId(nID);
}
int   ITimeline::GetId()
{
    return m_pTimelineImpl->GetId();
}
void  ITimeline::SetUserData(LPARAM lParam)
{
    m_pTimelineImpl->SetUserData(lParam);
}
LPARAM  ITimeline::GetUserData()
{
    return m_pTimelineImpl->GetUserData();
}
void  ITimeline::SetCustomBezierArgs(double d1, double d2, double d3, double d4)
{
    //m_pTimelineImpl->SetCustomBezierArgs(d1,d2,d3,d4)
}

//////////////////////////////////////////////////////////////////////////
#if 0
IIntTimeline::IIntTimeline(IntTimeline* p) :
    ITimeline(static_cast<Timeline*>(p)),
    m_pIntTimeline(p)
{
    
}

void  IIntTimeline::SetOutRef(int* pRef)
{
    m_pIntTimeline->SetOutRef(pRef);
}
void  IIntTimeline::SetEaseParam(int from, int to, int t, EaseType eType)
{
    m_pIntTimeline->SetEaseParam(from, to, t, eType);
}
void  IIntTimeline::SetLinerParam1(int from, int to, int t)
{
    m_pIntTimeline->SetLinerParam1(from, to, t);
}
void  IIntTimeline::SetAccelerateParam1(int from, int to, int t, float Vo)
{
    m_pIntTimeline->SetAccelerateParam1(from, to, t, Vo);
}


//////////////////////////////////////////////////////////////////////////

IFloatTimeline::IFloatTimeline(FloatTimeline* p) :
    ITimeline(static_cast<Timeline*>(p)),
    m_pIntTimeline(p)
{
    
}

void  IFloatTimeline::SetOutRef(float* pRef)
{
    m_pFloatTimeline->SetOutRef(pRef);
}
void  IFloatTimeline::SetEaseParam(float from, float to, float t, EaseType eType)
{
    m_pFloatTimeline->SetEaseParam(from, to, t, eType);
}
void  IFloatTimeline::SetLinerParam1(float from, float to, float t)
{
    m_pFloatTimeline->SetLinerParam1(from, to, t);
}
void  IFloatTimeline::SetAccelerateParam1(float from, float to, float t, float Vo)
{
    m_pFloatTimeline->SetAccelerateParam1(from, to, t, Vo);
}
#endif

IFromToTimeline::IFromToTimeline(FromToTimeline* p) : ITimeline(p)
{
	m_pFromToTimelineImpl = p;
}
IFromToTimeline*  IFromToTimeline::SetParam(float from, float to, float t)
{
	m_pFromToTimelineImpl->SetParam(from, to, t);
    return this;
}
void  IFromToTimeline::ModifyTo(float to)
{
	m_pFromToTimelineImpl->ModifyTo(to);
}

IFromToTimeline*  IFromToTimeline::SetEaseType(EaseType eType)
{
    m_pFromToTimelineImpl->SetEaseType(eType);
    return this;
}
IFromToTimeline*  IFromToTimeline::SetCustomBezierEase(
    double p1x, double p1y, double p2x, double p2y)
{
    m_pFromToTimelineImpl->SetCustomBezierEase(p1x,p1y,p2x,p2y);
    return this;
}
IFromToTimeline*  IFromToTimeline::SetCustomTimingFuction(pfnTimingFunction f)
{
    m_pFromToTimelineImpl->SetCustomTimingFuction(f);
    return this;
}


//////////////////////////////////////////////////////////////////////////

IKeyFrameTimeline::IKeyFrameTimeline(KeyFrameTimeline* p) : ITimeline(p)
{
	m_pKeyFrameTimelineImpl = p;
}

IKeyFrameTimeline*  IKeyFrameTimeline::SetParam(float from, float t)
{
	m_pKeyFrameTimelineImpl->SetParam(from, t);
	return this;
}

IKeyFrameTimeline*  IKeyFrameTimeline::AddKeyFrame(float percent, float to, EaseType eType)
{
	m_pKeyFrameTimelineImpl->AddKeyFrame(percent, to, eType);
	return this;
}


//////////////////////////////////////////////////////////////////////////

IStoryboard::IStoryboard(Storyboard* p) : m_pStoryboardImpl(p)
{
}
Storyboard* IStoryboard::GetImpl()
{
	return m_pStoryboardImpl;
}
ITimeline*  IStoryboard::FindTimeline(int nTimelineId)   
{ 
    Timeline* p = m_pStoryboardImpl->FindTimeline(nTimelineId);
	if (!p)
		return nullptr;

	return p->GetITimeline();
}
ITimeline*  IStoryboard::GetTimeline(unsigned int nIndex)
{
    Timeline* p = m_pStoryboardImpl->GetTimeline(nIndex); 
	if (!p)
		return nullptr;

	return p->GetITimeline();
}
// IIntTimeline*    IStoryboard::CreateIntTimeline(int nTimelineId)   
// {
//     return m_pStoryboardImpl->CreateIntTimeline(nTimelineId); 
// }
// IFloatTimeline*  IStoryboard::CreateFloatTimeline(int nTimelineId) 
// { 
//     return m_pStoryboardImpl->CreateFloatTimeline(nTimelineId); 
// }

IKeyFrameTimeline*  IStoryboard::CreateKeyFrameTimeline(int nId)
{
	KeyFrameTimeline* p = m_pStoryboardImpl->CreateKeyFrameTimeline(nId);
	if (p)
		return p->GetIKeyFrameTimeline();

	return nullptr;
}

ITimeline*   IStoryboard::CreateIdleTimeline(int nTimelineId)  
{
    IdleTimeline* p = m_pStoryboardImpl->CreateIdleTimeline(nTimelineId);
	if (p)
		return p->GetITimeline();

	return nullptr;
}
IFromToTimeline*  IStoryboard::CreateTimeline(int nId)
{
	FromToTimeline* p = m_pStoryboardImpl->CreateTimeline(nId);
	if (!p)
		return nullptr;

	return p->GetIFromToTimeline();
}

void  IStoryboard::Begin()                              
{ 
    m_pStoryboardImpl->Begin(); 
}
void  IStoryboard::BeginBlock()                          
{ 
    m_pStoryboardImpl->BeginBlock();
}
void  IStoryboard::BeginFullCpu()                         
{ 
	m_pStoryboardImpl->BeginFullCpu();
}

void  IStoryboard::BeginDelay(long lElapse)             
{
    m_pStoryboardImpl->BeginDelay(lElapse); 
}
void  IStoryboard::SetId(int nID)                        
{
    m_pStoryboardImpl->SetId(nID) ;
}
int   IStoryboard::GetId()                               
{
    return m_pStoryboardImpl->GetId();
}
bool  IStoryboard::IsFinish()                            
{
    return m_pStoryboardImpl->IsFinish();
}
void  IStoryboard::Cancel()
{
    m_pStoryboardImpl->Cancel();
}
void  IStoryboard::SetWParam(WPARAM wParam)              
{ 
    m_pStoryboardImpl->SetWParam(wParam);
}
WPARAM  IStoryboard::GetWParam()                         
{ 
    return m_pStoryboardImpl->GetWParam();
}
void  IStoryboard::SetLParam(LPARAM lParam)              
{
    m_pStoryboardImpl->SetLParam(lParam); 
}
LPARAM  IStoryboard::GetLParam()                      
{
    return m_pStoryboardImpl->GetLParam(); 
}


IAnimateManager::IAnimateManager(AnimateManager* p)
{
    m_pImpl = p;
}
AnimateManager*  IAnimateManager::GetImpl()
{
    return m_pImpl;
}

int IAnimateManager::SetFps(int n) 
{ 
    return m_pImpl->SetFps(n); 
}
void  IAnimateManager::OnTick()
{
    m_pImpl->OnTick();
}
void  IAnimateManager::ClearStoryboardByNotify(IAnimateEventCallback* p) 
{
    return m_pImpl->ClearStoryboardOfNotify(p); 
}
void  IAnimateManager::RemoveStoryboard(IStoryboard* p)
{
	if (p)
		m_pImpl->RemoveStoryboard(p->GetImpl()); 
}

void  IAnimateManager::RemoveStoryboardByNotityAndId(
        IAnimateEventCallback* pNotify, int nId)
{
    m_pImpl->RemoveStoryboardByNotityAndId(pNotify, nId);
}

IStoryboard*  IAnimateManager::CreateStoryboard(
                IAnimateEventCallback* pNotify, 
                int nId, 
                WPARAM wParam, 
                LPARAM lParam)
{
    Storyboard* p = m_pImpl->CreateStoryboard(pNotify, nId, wParam, lParam); 
	if (!p)
		return nullptr;

	return p->GetIStoryboard();
}

extern "C"
{
    void  CreateAnimateManager(
            IAnimateTimerCallback* p, 
            IAnimateManager** pp)
    {
        if (!p || !pp)
            return;

        AnimateManager* pAnimate = new AnimateManager;
        pAnimate->SetTimerCallback(p);
        *pp = pAnimate->GetIAnimateManager();
    }

    void  DestroyAnimateManager(
            IAnimateManager* p)
    {
        if (!p)
            return;

        delete p->GetImpl();
    }
}

}