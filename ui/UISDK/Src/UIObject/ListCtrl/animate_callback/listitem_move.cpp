#include "stdafx.h"
#include "listitem_move.h"
#include "include/interface/ilistctrlbase.h"
#include "..\ListItemBase\listitembase.h"

ListItemMoveAnimateImpl::ListItemMoveAnimateImpl()
{
    m_rcFrom.SetRectEmpty();
    m_rcTo.SetRectEmpty();
    m_easeType = UIA::ease_out;
    m_bFloatItem = false;
    m_lClassType = ListItemMoveAnimateImpl::CLASSTYPE;
}

ListItemMoveAnimateImpl::~ListItemMoveAnimateImpl()
{
    if (m_pListItem)
    {
        UI::LISTITEMSTYLE s = {0};
        s.bMoveAnimating = 1;
        m_pListItem->ModifyStyle(nullptr, &s);
    }
}

void  ListItemMoveAnimateImpl::SetFrom(LPCRECT prc)
{
    UIASSERT(prc);
    m_rcFrom.CopyRect(prc);
}
void  ListItemMoveAnimateImpl::SetTo(LPCRECT prc)
{
	UIASSERT(prc);
    m_rcTo.CopyRect(prc);
}
void  ListItemMoveAnimateImpl::SetEaseType(UIA::EaseType e)
{
    m_easeType = e;
}

bool ListItemMoveAnimateImpl::StartReq(UIA::IStoryboard* pStoryboard)
{
    if (m_rcFrom.IsRectEmpty() || m_rcTo.IsRectEmpty())
        return false;

    UI::IListCtrlBase* pListCtrl = GetListCtrl();
    if (!pListCtrl)
        return false;

    m_bFloatItem = m_pListItem->IsFloat();

    UIA::IFromToTimeline* pTimelineX = pStoryboard->CreateTimeline(LEFT);
    pTimelineX->SetParam(
        (float)m_rcFrom.left,
        (float)m_rcTo.left,
        (float)m_lDuration);
    pTimelineX->SetEaseType(m_easeType);

    UIA::IFromToTimeline* pTimelineY = pStoryboard->CreateTimeline(TOP);
    pTimelineY->SetParam(
        (float)m_rcFrom.top,
        (float)m_rcTo.top,
        (float)m_lDuration);
    pTimelineY->SetEaseType(m_easeType);

    if (m_rcFrom.Width() != m_rcTo.Width() || 
        m_rcFrom.Height() != m_rcTo.Height())
    {
        UIA::IFromToTimeline* pTimelineX = pStoryboard->CreateTimeline(RIGHT);
        pTimelineX->SetParam(
            (float)m_rcFrom.right,
            (float)m_rcTo.right,
            (float)m_lDuration);
        pTimelineX->SetEaseType(m_easeType);

        UIA::IFromToTimeline* pTimelineY = pStoryboard->CreateTimeline(BOTTOM);
        pTimelineY->SetParam(
            (float)m_rcFrom.bottom,
            (float)m_rcTo.bottom,
            (float)m_lDuration);
        pTimelineY->SetEaseType(m_easeType);
    }

    UI::LISTITEMSTYLE s = {0};
    s.bMoveAnimating = 1;
    m_pListItem->ModifyStyle(&s, nullptr);

    return true;
}

void ListItemMoveAnimateImpl::OnEnd(UIA::E_ANIMATE_END_REASON)
{
	// 不处理。被中断的动画应该由中断调用者来负责设置新的位置。否则在这里
	// 直接到到m_rcTo可能会造成界面闪烁。
// 	if (!m_bFloatItem)
// 		m_pListItem->SetParentRect(&m_rcTo);
}

UIA::E_ANIMATE_TICK_RESULT ListItemMoveAnimateImpl::OnTick(UIA::IStoryboard* pStoryboard)
{
    if (!m_pListItem)
        return UIA::ANIMATE_TICK_RESULT_CANCEL;

    RECT rect = {0};
    rect.left = pStoryboard->GetTimeline(LEFT)->GetCurrentIntValue();
    rect.top = pStoryboard->GetTimeline(TOP)->GetCurrentIntValue();

    rect.right = rect.left + m_rcTo.Width();
    rect.bottom = rect.top + m_rcTo.Height();

	UIA::ITimeline* pTimeline = pStoryboard->GetTimeline(RIGHT);
    if (pTimeline)
        rect.right = pTimeline->GetCurrentIntValue();
    pTimeline = pStoryboard->GetTimeline(BOTTOM);
    if (pTimeline)
        rect.bottom = pTimeline->GetCurrentIntValue();

    if (m_bFloatItem)
    {
        m_pListItem->SetFloatRect(&rect);
    }
    else
    {
        m_pListItem->SetParentRectLightly(&rect);
    }
    GetListCtrl()->Invalidate();
    return UIA::ANIMATE_TICK_RESULT_CONTINUE;
}

void ListItemMoveAnimateImpl::Cancel(UI::ListItemBase& item)
{
	ListItemMoveAnimateImpl temp;
	temp.Bind(*item.GetIListItemBase());
	AnimateWrapManager::Get().MakesureUnique(&temp);
}

bool  ListItemMoveAnimateImpl::IsEqual(AnimateImpl* p)
{
    if (p->GetClassType() == ListItemMoveAnimateImpl::CLASSTYPE)
    {
        return ListItemAnimateImpl::IsEqual(
                    static_cast<ListItemAnimateImpl*>(p));
    }
    return false;
}
