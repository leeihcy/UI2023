#include "stdafx.h"
#include "timermanager.h"

namespace ui
{

TimerHelper* TimerHelper::GetInstance()
{
    static TimerHelper s;
    return &s;
}

TimerHelper::TimerHelper()
{
    // m_thunk.Init((DWORD_PTR)TimerHelper::TimerProc, this);
}

UINT_PTR  TimerHelper::SetTimer(int nElapse, IMessage* pNotify)
{
    if (!pNotify)
        return 0;

    TimerItem  ti;
    ti.pNotify = pNotify;
    return this->SetTimer(nElapse, &ti);
}

UINT_PTR  TimerHelper::SetTimer(int nElapse, std::function<bool(UINT_PTR, TimerItem*)> func)
{
	if (!func)
		return 0;

	TimerItem  ti;
	ti.func = func;
	return this->SetTimer(nElapse, &ti);
}

UINT_PTR  TimerHelper::SetTimerById(int nElapse, UINT_PTR nId, IMessage* pNotify)
{
    if (nullptr == pNotify)
        return 0;

    // 查找该pNotify对应的nId是否已经存在
    _MyIter iter = m_mapTimerItem.begin();
    for (; iter != m_mapTimerItem.end(); iter++)
    {
        if (iter->second.nId == nId && iter->second.pNotify == pNotify)
            return 0;
    }

    TimerItem  ti;
    ti.nId = nId;
    ti.pNotify = pNotify;
    return this->SetTimer(nElapse, &ti);
}

UINT_PTR  TimerHelper::SetTimer(int nElapse, TimerItem* pItem)
{
    if (nullptr == pItem)
        return 0;

    UINT_PTR nTimerID = ::SetTimer(nullptr, 0, nElapse, TimerHelper::TimerProc);
    m_mapTimerItem[nTimerID] = *pItem;

    // 防止崩溃
    TimerItem& itemRef = m_mapTimerItem[nTimerID];
    if (itemRef.pNotify)
        itemRef.pNotify->AddDelayRef((void**)&itemRef.pNotify);

//    int nCount = m_mapTimerItem.size();
//    UI_LOG_DEBUG(_T("%s  timer id:%d, nElapse=%d, id=%d, pNotify=0x%x"), FUNC_NAME, nTimerID, nElapse, pItem->nId, pItem->pNotify);
    return nTimerID;
}
void  TimerHelper::KillTimer(UINT_PTR& nTimerIdRef)
{
    _MyIter iter = m_mapTimerItem.find(nTimerIdRef);
    if (iter != m_mapTimerItem.end())
    {
//        UI_LOG_DEBUG(_T("%s timer id:%d"), FUNC_NAME, nTimerIdRef);

        if (iter->second.pNotify)
            iter->second.pNotify->RemoveDelayRef((void**)&iter->second.pNotify);

        ::KillTimer(0, nTimerIdRef);

        m_mapTimerItem.erase(iter);
        nTimerIdRef = 0;
        return;
    }
}

void  TimerHelper::KillTimerById(int nId, IMessage* pNotify)
{
    _MyIter iter = m_mapTimerItem.begin();
    for (; iter != m_mapTimerItem.end(); iter++)
    {
        if (iter->second.nId == nId && iter->second.pNotify == pNotify)
        {
//            UI_LOG_DEBUG(_T("%s timer id:%d, id=%d"), FUNC_NAME, iter->first, nId);
            if (iter->second.pNotify)
                iter->second.pNotify->RemoveDelayRef((void**)&iter->second.pNotify);

            ::KillTimer(nullptr, iter->first);
            m_mapTimerItem.erase(iter);
            return;
        }
    }
}
void  TimerHelper::KillTimerByNotify(IMessage* pNotify)
{
    _MyIter iter = m_mapTimerItem.begin();
    for (; iter != m_mapTimerItem.end();)
    {
        if (iter->second.pNotify == pNotify)
        {
            if (iter->second.pNotify)
                iter->second.pNotify->RemoveDelayRef((void**)&iter->second.pNotify);

//            UI_LOG_DEBUG(_T("%s timer id:%d, id=%d"), FUNC_NAME, iter->first, nId);
            ::KillTimer(nullptr, iter->first);
            iter = m_mapTimerItem.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

VOID CALLBACK  TimerHelper::TimerProc( HWND hwnd, unsigned int uMsg, UINT_PTR idEvent,	DWORD dwTime )
{
    TimerHelper::GetInstance()->OnTimer(idEvent);
}
void  TimerHelper::OnTimer(UINT_PTR idEvent)
{
    if (0 == m_mapTimerItem.count(idEvent))
    {
        UIASSERT(0);
        TimerHelper::GetInstance()->KillTimer(idEvent);
        return;
    }
    TimerItem& ti = m_mapTimerItem[idEvent];

    if (-1 != ti.nRepeatCount)
        ti.nRepeatCount --;

	int nRepeatCount = ti.nRepeatCount;
    if (ti.pNotify)
    {
        ::UISendMessage(ti.pNotify, WM_TIMER, idEvent, (LPARAM)&ti);
    }
	else if (ti.pProc)
	{
		ti.pProc(idEvent, &ti);
	}
	else if (ti.func)
	{
		// 返回false表示不再继续，后面删除该定时器
		if (false == ti.func(idEvent, &ti))
		{
			nRepeatCount = 0;
		}
	}

	// 注：ti可能已被销毁，不能再访问了
	if (0 == nRepeatCount)
	{
		KillTimer(idEvent);
	}
}

}