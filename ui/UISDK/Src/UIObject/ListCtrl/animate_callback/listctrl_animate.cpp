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

		// float��λ����float rect������������parent rect
		if (!item.IsFloat())
		{
			// ����rect�����ɼ��Ļ�����������
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
		// �жϿ��ܻ��������еĶ���
		UI::LISTITEMSTYLE s = { 0 };
		s.bMoveAnimating = 1;
		if (item.TestStyle(s))
		{
			ListItemMoveAnimateImpl::Cancel(item);
		}
	}

	return bAnimate;
}
