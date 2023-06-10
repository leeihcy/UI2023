#include "stdafx.h"
#include "..\ListItemBase\listitembase.h"
#include "listctrl_animate.h"
#include "listitem_move.h"
#include "Inc\Interface\ilistctrlbase.h"
#include "..\..\Window\windowbase.h"
#include "..\listctrlbase.h"

ListCtrlAnimate::ListCtrlAnimate()
{
    m_pfnListItemRectChangedCallback = nullptr;
}
ListCtrlAnimate::~ListCtrlAnimate()
{

}

ListCtrlAnimate&  ListCtrlAnimate::Get()
{
    static ListCtrlAnimate s;
    return s;
}

bool  ListCtrlAnimate::HandleItemRectChanged(
           ListItemBase& item,
           LPCRECT prcOld, 
           LPCRECT prcNew)
{
	UIASSERT(prcOld && prcNew);

	ListCtrlBase* listctrl = item.GetListCtrlBase();
	WindowBase* window = nullptr;
	if (listctrl)
		window = listctrl->GetWindowObject();
	if (window && !window->IsGpuComposite())
		return false;

	if (::EqualRect(prcNew, prcOld))
	{
		UI::LISTITEMSTYLE s = { 0 };
		s.bMoveAnimating = 1;
		if (item.TestStyle(s))
		{
			ListItemMoveAnimateImpl::Cancel(item);
		}

		return false;
	}

	UI::IListCtrlBase* pListCtrl = item.GetIListCtrlBase();
	if (!pListCtrl)
		return false;

	bool bAnimate = false;
	do
	{
		if (IsRectEmpty(prcOld))
			break;

		// float的位置由float rect决定，而不是parent rect
		if (!item.IsFloat())
		{
			// 两个rect都不可见的话，不做动画
			if (!pListCtrl->IsItemRectVisibleInScreen(prcOld) &&
				!pListCtrl->IsItemRectVisibleInScreen(prcNew))
			{
				break;
			}
		}

		if (!item.IsVisible())
			break;

		if (!pListCtrl->IsVisible())
			break;

		bAnimate = true;
	} while (0);

	if (bAnimate)
	{
		ListItemMoveAnimateImpl* ani = new ListItemMoveAnimateImpl;
		ani->Bind(*item.GetIListItemBase());
		ani->SetFrom(prcOld);
		ani->SetTo(prcNew);

// 		if (item.IsFloat())
// 			ani->SetEndCallback(ListItemAnimateCallback, 0);

		bAnimate = ani->Start(false);
	}

	if (!bAnimate)
	{
		// 中断可能还在运行中的动画
		UI::LISTITEMSTYLE s = { 0 };
		s.bMoveAnimating = 1;
		if (item.TestStyle(s))
		{
			ListItemMoveAnimateImpl::Cancel(item);
		}
	}

	return bAnimate;
}
