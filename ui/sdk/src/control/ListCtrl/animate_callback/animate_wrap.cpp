#include "stdafx.h"
#include "animate_wrap.h"
#include <algorithm>


AnimateWrapManager::AnimateWrapManager()
{
    m_lDefaultDuration = 200;
    m_eDefaultEaseType = UIA::ease_out;
}

AnimateWrapManager& AnimateWrapManager::Get()
{
    static AnimateWrapManager s;
    return s;
}

long  AnimateWrapManager::GetDefaultDuration()
{
    return m_lDefaultDuration;
}
UIA::EaseType  AnimateWrapManager::GetDefaultEaseType()
{
    return m_eDefaultEaseType;
}

void  AnimateWrapManager::AddAnimate(AnimateImpl* p)
{
    m_listAnimate.push_back(p);
}

void  AnimateWrapManager::RemoveAnimate(AnimateImpl* p)
{
    list<AnimateImpl*>::iterator iter = 
            std::find(m_listAnimate.begin(), m_listAnimate.end(), p);
    
    if (iter != m_listAnimate.end())
    {
        m_listAnimate.erase(iter);
    }
}

// ���⶯������
void  AnimateWrapManager::MakesureUnique(AnimateImpl* pUnique)
{
    list<AnimateImpl*>::iterator iter = m_listAnimate.begin();
    for (; iter != m_listAnimate.end(); ++iter)
    {
        if ((*iter)->CheckEqual(pUnique))
        {
            AnimateImpl* p = *iter;
            p->CancelAnimate();
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////


AnimateImpl::AnimateImpl()
{
    m_lUniqueId = 0;
    m_pStoryboard = nullptr;
    m_lDuration = AnimateWrapManager::Get().GetDefaultDuration();
    m_lClassType = 0;
}

AnimateImpl::~AnimateImpl()
{
    AnimateWrapManager::Get().RemoveAnimate(this);

	// ���ﲻ���������ٶ�������ΪOnAnimateCancel���ֻᴥ��delete this;
    UIASSERT (!m_pStoryboard);
}

void  AnimateImpl::SetUniqueId(long l)
{
    m_lUniqueId = l;
}
long  AnimateImpl::GetUniquieId()
{
    return m_lUniqueId;
}

long  AnimateImpl::GetClassType()
{
    return m_lClassType;
}

void  AnimateImpl::SetDuration(long l)
{
    m_lDuration = l;
}

bool  AnimateImpl::PreStart()
{
    return true;
}

bool  AnimateImpl::IsEqual(AnimateImpl* p)
{
    return false;
}

bool  AnimateImpl::CheckEqual(AnimateImpl* p)
{
    UIASSERT (p);
    if (m_lUniqueId)
    {
        return p->m_lUniqueId == m_lUniqueId ? true:false;
    }

    return IsEqual(p);
}

bool  AnimateImpl::Start(bool bBlock)
{
	if (!PreStart())
		return false;

    AnimateWrapManager::Get().MakesureUnique(this);

    // ����m_pStoryboard
    UIASSERT(nullptr == m_pStoryboard);

    // ��������Ϣ�������ò���app����˽�������ȥ���
    UI::IApplication* pUIApp = this->GetUIApplication();
    if (!pUIApp)
        return false;

    UIA::IAnimate* pAnimateMgr = pUIApp->GetAnimateMgr();
    if (!pAnimateMgr)
        return false;

    m_pStoryboard = pAnimateMgr->CreateStoryboard(
            static_cast<UIA::IAnimateEventCallback*>(this));

    if (StartReq(m_pStoryboard))
    {
        if (!bBlock)
        {
            AnimateWrapManager::Get().AddAnimate(this);
            m_pStoryboard->Begin();
            return true;
        }
        else
        {
            m_pStoryboard->BeginBlock();
        }
    }
    else
    {
        pAnimateMgr->RemoveStoryboard(m_pStoryboard);
        m_pStoryboard = nullptr;
    }
    return false;
}

void  AnimateImpl::CancelAnimate()
{
    if (!m_pStoryboard)
	{
		delete this;
        return;
	}

    m_pStoryboard->Cancel();
    // ����OnAnimateEnd��this �������ˣ������ٵ���
    return;
}

void  AnimateImpl::OnAnimateEnd(UIA::IStoryboard* p, UIA::eAnimateEndReason e)
{
    UIASSERT(m_pStoryboard = p);

    this->OnEnd(e);
    m_pStoryboard = nullptr;

	PostEnd();
    delete this;
}

UIA::eAnimateTickResult  AnimateImpl::OnAnimateTick(UIA::IStoryboard* p)
{
    UIASSERT (m_pStoryboard = p);

    return this->OnTick(p);
}
