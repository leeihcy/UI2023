#include "stdafx.h"
#include "listctrl_inner_drag.h"
#include <assert.h>
#include "..\listctrlbase.h"
#include "..\ListItemBase\listitembase.h"
#include "..\animate_callback\listctrl_animate.h"


enum{
	ANIMATE_UNIQUE_ID = 163162232
};


ListCtrlInnerDragMgr::ListCtrlInnerDragMgr()
{
	m_pItemDrag = nullptr;
	m_ptPress.x = m_ptPress.y = -32000;
	m_nOldHScrollPos = 0;
	m_nOldVScrollPos = 0;
	SetRectEmpty(&m_rcOldPressItem);
	m_nOldPressItemIndex = 0;
	m_bDraging = false;
	m_bLButtondown = false;
}

ListCtrlInnerDragMgr::~ListCtrlInnerDragMgr()
{
}

BOOL UI::ListCtrlInnerDragMgr::ProcessMessage(UIMSG* pMsg)
{
	BOOL bHandled = FALSE;
	switch (pMsg->message)
	{
	case WM_MOUSEMOVE:
		{
			bHandled = OnDragMouseMove(pMsg->wParam, pMsg->lParam);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			OnDragLButtonDown(pMsg->wParam, pMsg->lParam);
		}
		break;
	case WM_LBUTTONUP:
		{
			bHandled = OnDragLButtonUp(pMsg->wParam, pMsg->lParam);
		}
		break;
	case WM_KEYDOWN:
		{
			if (pMsg->wParam == VK_ESCAPE)
			{
                if (m_bDraging)
                {
				    CancelDrag();
                    bHandled = TRUE;
                }
			}
		}
		break;

	case WM_KILLFOCUS:
	case WM_CANCELMODE:
		{
			if (m_bLButtondown)
				m_bLButtondown = FALSE;

			AcceptDrag();
		}
		break;
	}
	return bHandled;
}
void  ListCtrlInnerDragMgr::SetListCtrlBase(ListCtrlBase*  pCtrl)
{
    m_pListCtrlBase = pCtrl;
}

void  ListCtrlInnerDragMgr::OnRemoveAll()
{
    __EndDrag();
}

void  ListCtrlInnerDragMgr::OnRemoveItem(ListItemBase* pItem)
{
    if (pItem == m_pItemDrag)
        m_pItemDrag = nullptr;

    CancelDrag();
}

BOOL UI::ListCtrlInnerDragMgr::OnDragMouseMove(WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled = FALSE;
	if (!m_bLButtondown)
		return FALSE;

	POINT ptNow = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

	if (!m_bDraging)
	{
        // 有可能是拖拽完成了，但动画还没有结束。
        if (m_pItemDrag)
            return bHandled;

		if (abs(ptNow.x-m_ptPress.x) > 5 ||
			abs(ptNow.y-m_ptPress.y) > 5)
		{
			bHandled = TRUE;
			StartDrag(ptNow);
		}
	}
	else
	{
		bHandled = TRUE;
		Draging(ptNow);
	}
	return bHandled;
}

void UI::ListCtrlInnerDragMgr::OnDragLButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_ptPress.x = GET_X_LPARAM(lParam);
	m_ptPress.y = GET_Y_LPARAM(lParam);
	m_bLButtondown = true;
	m_nOldHScrollPos = m_pListCtrlBase->GetScrollOffset(
			&m_nOldHScrollPos, &m_nOldVScrollPos);
}

BOOL UI::ListCtrlInnerDragMgr::OnDragLButtonUp(WPARAM wParam, LPARAM lParam)
{
    BOOL  bHandled = FALSE;

	if (m_bDraging)
	{
		AcceptDrag();

        //bHandled = TRUE;  // <-- 不能设置为TRUE，还得交给mk去释放press item
	}
	m_bLButtondown = false;

    return bHandled;
}

void UI::ListCtrlInnerDragMgr::StartDrag(POINT pt)
{
	ListItemBase*  pPressItem = m_pListCtrlBase->GetPressItem();
	if (!pPressItem)
		return;

    if (pPressItem->IsFloat())
        return;

	//CListItemMoveAnimate::Cancel(*pPressItem);
	LISTITEMSTYLE s = {0};
	s.bMoveAnimating = 1;
	if (pPressItem->TestStyle(s))
		return;

	m_pItemDrag = pPressItem;

    // 清空按下的控件，避免开始拖拽时，如果鼠标点在控件上面，
    // 拖拽结束时又会触发点击控件的操作。
    if (m_pListCtrlBase->GetMKManager())
        m_pListCtrlBase->GetMKManager()->SetPressObject(nullptr);

	m_bDraging = true;

    CRect rc2Float;
        
    m_pItemDrag->GetParentRect(&rc2Float);
    m_pListCtrlBase->ItemRect2ObjectRect(&rc2Float, &rc2Float);
    m_pItemDrag->SetFloat(true);
    m_pItemDrag->SetFloatRect(&rc2Float);

    m_rcOldPressItem = rc2Float;
}

//
// 1. 根据rcFloat调整列表滚动条位置
// 2. 限制rcFloat在客户区域之内
//
void  update_scrollpos_by_float_rect(
            CRect& rcFloat, 
            ListCtrlBase* pListCtrl, 
            bool& bScrollPosChanged)
{
    bScrollPosChanged = false;

    CRect rcClient;
    pListCtrl->GetIListCtrlBase()->GetClientRectInObject(&rcClient);

    if (rcFloat.top < rcClient.top)
    {
        if (pListCtrl->ScrollY(-10, false))
        {
            bScrollPosChanged = true;
        }

        OffsetRect(&rcFloat, 0, rcClient.top - rcFloat.top);
    }
    else if (rcFloat.bottom > rcClient.bottom)
    {
        if (pListCtrl->ScrollY(10, false))
        {
            bScrollPosChanged = true;
        }

        OffsetRect(&rcFloat, 0, rcClient.bottom - rcFloat.bottom);
    }

    if (rcFloat.left < rcClient.left)
    {
        if (pListCtrl->ScrollX(-10, false))
        {
            bScrollPosChanged = true;
        }

        OffsetRect(&rcFloat, rcClient.left - rcFloat.left, 0);
    }
    else if (rcFloat.right > rcClient.right)
    {
        if (pListCtrl->ScrollX(10, false))
        {
            bScrollPosChanged = true;
        }

        OffsetRect(&rcFloat, rcClient.right - rcFloat.right, 0);
    }

    if (bScrollPosChanged)
    {
        pListCtrl->SetCalcFirstLastDrawItemFlag();
    }
}

// 判断rcIntersect是否超过了rcItem的一半位置
bool  test_intersect_more_than_half(CRect& rcItem, CRect& rcIntersect)
{
    CPoint ptCenter = rcItem.CenterPoint();

    if (rcIntersect.right <= ptCenter.x)
        return false;

    if (rcIntersect.left >= ptCenter.x)
        return false;

    if (rcIntersect.bottom <= ptCenter.y)
        return false;

    if (rcIntersect.top >= ptCenter.y)
        return false;
    
    return true;
}

// 计算新位置是向前走了，还是向后走了。
// 向前走了，则要用 insert before
// 向后走了，则要用 insert after
enum HIT_ITEM_RELATIVE_POS
{
    INVALID_POS,
    BEFORE_ME,
    AFTER_ME
};

//
// 检测当前拖拽位于与哪个item最相交
// 注：这里默认所有的item大小是一样的
//
ListItemBase*  drag_hit_test(
                    CRect* prcDragParent, 
                    ListItemBase* pDragItem, 
                    ListCtrlBase* pListCtrl,
                    HIT_ITEM_RELATIVE_POS& ePos)
{
    assert (pDragItem && pListCtrl);
    ePos = INVALID_POS;

    CRect rcItem;
    CRect rcIntersect;

    ListItemBase* p = pListCtrl->GetFirstDrawItem();
    ListItemBase* pLast = pListCtrl->GetLastDrawItem();

    if (!p || !pLast)
        return nullptr;

    pLast = pLast->GetNextVisibleItem();
    ePos = BEFORE_ME;

    LISTITEMSTYLE s = {0};
    s.bMoveAnimating = 1;

    for (; p != pLast; p = p->GetNextVisibleItem())
    {
        if (p == pDragItem)
        {
            ePos = AFTER_ME;
            continue;;
        }

        // 还在动的item不处理
        if (p->TestStyle(s))
            continue;

        p->GetParentRect(&rcItem);
        rcIntersect.IntersectRect(&rcItem, prcDragParent);

        // 判断相交情况
        if (!test_intersect_more_than_half(rcItem, rcIntersect))
            continue;

        return p;
    }

    ePos = INVALID_POS;
    return nullptr;
}

void  ListCtrlInnerDragMgr::Draging(POINT pt)
{
    // 计算拖拽项的位置
    int nyMove = pt.y - m_ptPress.y;
    int nxMove = pt.x - m_ptPress.x;

    CRect rcNewFloat = m_rcOldPressItem;
    OffsetRect(&rcNewFloat, nxMove, nyMove);

    // 边缘滚动
    bool bScrollPosChanged = false;
    update_scrollpos_by_float_rect(
            rcNewFloat, 
            m_pListCtrlBase, 
            bScrollPosChanged);

    // 调整拖拽项的位置
    CRect rcOldFloat;
    m_pItemDrag->GetFloatRect(&rcOldFloat);
	
	// 先屏蔽这一限制。会导致item移到边界时，鼠标在外部移动也不触发hittest，但此
	// 时可能该item下面还有一个item，却不会将该item挤开。
//     if (rcOldFloat.EqualRect(&rcNewFloat))
//     {
//         if (bScrollPosChanged)
//             m_pListCtrlBase->GetIListCtrlBase()->Invalidate();
//         
//         return;
//     }

    m_pItemDrag->SetFloatRect(&rcNewFloat);

    // 计算要占据的位置
    CRect  rcNewDragItemRect;
    m_pListCtrlBase->ObjectRect2ItemRect(&rcNewFloat, &rcNewDragItemRect);

    HIT_ITEM_RELATIVE_POS ePos = INVALID_POS;
    ListItemBase* pHitTest = drag_hit_test(
                    &rcNewDragItemRect, 
                    m_pItemDrag, 
                    m_pListCtrlBase,
                    ePos);
    if (!pHitTest)
    {
        m_pListCtrlBase->GetIListCtrlBase()->Invalidate();
        return;
    }

    // 调整位置
    IListItemBase* pInsertAfter = pHitTest->GetIListItemBase();
    if (ePos == BEFORE_ME)
    {
        ListItemBase* pPrev = pHitTest->GetPrevVisibleItem();
        if (pPrev)
            pInsertAfter = pPrev->GetIListItemBase();
        else
            pInsertAfter = UITVI_FIRST;
    }
    
    if (m_pListCtrlBase->MoveItem(
                m_pItemDrag, 
                nullptr,
                pInsertAfter))
    {
        m_pListCtrlBase->LayoutItem(nullptr, false);
    }

    // 刷新
    m_pListCtrlBase->GetIListCtrlBase()->Invalidate();
}

void  FloatItemAnimateEndCallback(UI::ListItemBase* pItem, long* pData)
{
    if (pItem)
    {
        pItem->SetFloat(false);
    }
}

void  ListCtrlInnerDragMgr::AcceptDrag()
{
	if (!m_bDraging || !m_pItemDrag)
		return;

	CRect rcFrom;
	m_pItemDrag->GetFloatRect(&rcFrom);

	CRect rcTo;
	m_pItemDrag->GetParentRect(&rcTo);

	m_pListCtrlBase->ItemRect2ObjectRect(&rcTo, &rcTo);

	if (!ListCtrlAnimate::HandleItemRectChanged(
			*m_pItemDrag, &rcFrom, &rcTo))
	{
		m_pItemDrag->SetFloat(false);
	}


//     LISTCTRL_INNERDRAG_FINISH_DATA  data;
//     data.pDragItem = m_pItemDrag->GetIListItemBase();

    __EndDrag();

// 	UIMSG  msg;
// 	msg.message = UI_MSG_NOTIFY;
// 	msg.nCode = UI_LCN_INNERDRAG_FINISH;
// 	msg.pMsgFrom = m_pListCtrlBase->GetIListCtrlBase();
// 	msg.wParam = (WPARAM)&data;
// 	msg.pMsgFrom->DoNotify(&msg);
}

void UI::ListCtrlInnerDragMgr::CancelDrag()
{
	if (!m_bDraging)
		return;

	int nOldHScrollPos = m_nOldHScrollPos;
	int nOldVScrollPos = m_nOldVScrollPos;

    // 先UpdateItemRect，然后再启动动画。
    // 因为在UpdateItemRect中发现parent rect一致的话，
    // 会取消当前动画
    m_pListCtrlBase->LayoutItem(nullptr, false);

    if (m_pItemDrag)
    {
		CRect rcFrom;
		m_pItemDrag->GetFloatRect(&rcFrom);

		if (!ListCtrlAnimate::HandleItemRectChanged(
					*m_pItemDrag, &rcFrom, &m_rcOldPressItem))
		{
			m_pItemDrag->SetFloat(false);
		}
    }

	__EndDrag();

    m_pListCtrlBase->SetScrollPos(nOldHScrollPos, nOldVScrollPos, false);
	m_pListCtrlBase->GetIListCtrlBase()->Invalidate();
}

void UI::ListCtrlInnerDragMgr::__EndDrag()
{
    m_pItemDrag = nullptr;
	m_bDraging = false;
	m_bLButtondown = false;
	m_ptPress.x = m_ptPress.y = -32000;
	m_nOldHScrollPos = 0;
	m_nOldVScrollPos = 0;
	m_nOldPressItemIndex = 0;
}

bool UI::ListCtrlInnerDragMgr::IsDraging()
{
	return m_bDraging;
}

