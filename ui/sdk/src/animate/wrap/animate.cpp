#include "include/inc.h"
#include "animate.h"
#include "../animatemgr.h"

namespace ui {

AnimateHelper::AnimateHelper(Application& app) : m_uiApplication(app)
{
    m_pWaitforHandle = nullptr;
    m_pAnimateMgr = nullptr;
}

AnimateHelper::~AnimateHelper()
{
    UIASSERT(!m_pAnimateMgr);
}

void  AnimateHelper::Init(WaitForHandlesMgr* p)
{
    m_pWaitforHandle = p;
    uia::CreateAnimateManager(this, &m_pAnimateMgr);
	m_pAnimateMgr->GetImpl()->SetUIApplication(&m_uiApplication);
}

void  AnimateHelper::UnInit()
{
    uia::DestroyAnimateManager(m_pAnimateMgr);
    m_pAnimateMgr = nullptr;
}

void  AnimateHelper::OnSetTimer(long hHandle)
{
#if 0
    if (m_pWaitforHandle)
    {
        m_pWaitforHandle->AddHandle(
            hHandle, 
            static_cast<IWaitForHandleCallback*>(this), 
            0);
    }
#endif
}

void  AnimateHelper::OnKillTimer(long hHandle)
{
#if 0
    if (m_pWaitforHandle)
    {
        m_pWaitforHandle->RemoveHandle(hHandle);
    }
#endif
}

// void AnimateHelper::OnWaitForHandleObjectCallback(long, long)
// {
//     if (m_pAnimateMgr)
//         m_pAnimateMgr->OnTick();
// }

uia::IAnimateManager*  AnimateHelper::GetAnimateManager()
{
    return m_pAnimateMgr;
}

}