#include "stdafx.h"
#include "animate.h"
#include "src/Helper\msg\msghelper.h"
#include "..\animatemgr.h"

AnimateHelper::AnimateHelper(UIApplication& app) : m_uiApplication(app)
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
    UIA::CreateAnimateManager(this, &m_pAnimateMgr);
	m_pAnimateMgr->GetImpl()->SetUIApplication(&m_uiApplication);
}

void  AnimateHelper::UnInit()
{
    UIA::DestroyAnimateManager(m_pAnimateMgr);
    m_pAnimateMgr = nullptr;
}

void  AnimateHelper::OnSetTimer(HANDLE hHandle)
{
    if (m_pWaitforHandle)
    {
        m_pWaitforHandle->AddHandle(
            hHandle, 
            static_cast<IWaitForHandleCallback*>(this), 
            0);
    }
}

void  AnimateHelper::OnKillTimer(HANDLE hHandle)
{
    if (m_pWaitforHandle)
    {
        m_pWaitforHandle->RemoveHandle(hHandle);
    }
}

void AnimateHelper::OnWaitForHandleObjectCallback(HANDLE, LPARAM)
{
    if (m_pAnimateMgr)
        m_pAnimateMgr->OnTick();
}

UIA::IAnimateManager*  AnimateHelper::GetAnimateManager()
{
    return m_pAnimateMgr;
}
