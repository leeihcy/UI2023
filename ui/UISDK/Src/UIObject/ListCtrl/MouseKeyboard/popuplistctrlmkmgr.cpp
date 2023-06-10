#include "stdafx.h"
#include "popuplistctrlmkmgr.h"
#include "Src\UIObject\ListCtrl\listctrlbase.h"
#include "Src\UIObject\ListCtrl\ListItemBase\listitembase.h"
#include "Src\Base\Application\uiapplication.h"

using namespace UI;

PopupListCtrlMKMgr::PopupListCtrlMKMgr()
{
    m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
}

BOOL  PopupListCtrlMKMgr::DoProcessMessage(UIMSG* pMsg)
{
    bool bInterestMsg = true;
    switch (pMsg->message)
    {
    case WM_MOUSEMOVE:
        OnMouseMove(pMsg);
        break;

    case WM_MOUSELEAVE:
        OnMouseLeave(pMsg);
        break;

    case WM_LBUTTONDOWN:
        OnLButtonDown(pMsg);
        break;

    case WM_LBUTTONUP:
        OnLButtonUp(pMsg);
        break;

    case WM_KEYDOWN:
        OnKeyDown(pMsg, &bInterestMsg);
        break;

    default:
        return __super::DoProcessMessage(pMsg);
        break;
    }

    if (bInterestMsg)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void  PopupListCtrlMKMgr::OnMouseMove(UIMSG* pMsg)
{
    POINT  ptWindow = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };

    // 其它原因发送过来的WM_MOUSEMOVE消息，不处理，避免影响使用上下方向键切换当前项
    if (ptWindow.x == m_ptLastMousePos.x && ptWindow.y == m_ptLastMousePos.y) 
        return;
    m_ptLastMousePos = ptWindow;

    ListItemBase*  pNewHover = GetItemByPos(ptWindow);

    if (pNewHover != m_pItemHover)
    {
        if (nullptr == pNewHover)  // 不取消当前高亮，除非有一个新的hover对象
        {
            if (m_pUIApplication)
                m_pUIApplication->HideToolTip();

            return;  
        }

        SetHoverItem(pNewHover);

        ListItemBase* pSelItem = m_pListCtrlBase->GetFirstSelectItem();
        if (pSelItem)
            this->m_pListCtrlBase->InvalidateItem(pSelItem);  // 当有hover项时，不显示selected项
    }
    if (m_pItemHover)
    {
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
    }	
}

void  PopupListCtrlMKMgr::OnMouseLeave(UIMSG* pMsg)
{
    if (m_pItemHover)
    {
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_MOUSELEAVE, pMsg->wParam, pMsg->lParam);
        if (m_pUIApplication)
            m_pUIApplication->HideToolTip();
    }
}

// 鼠标按下时不选中，弹起时才选中
void  PopupListCtrlMKMgr::OnLButtonUp(UIMSG* pMsg)
{
    BOOL bHandled = FALSE;
    if (m_pItemPress)
    {
        ListItemBase* pSave = m_pItemPress;
        this->SetPressItem(nullptr);
		LONG_PTR lRet = UISendMessage(pSave->GetIListItemBase(), WM_LBUTTONUP, pMsg->wParam, pMsg->lParam,0, 0,0, &bHandled);

        POINT pt = {0};
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pListCtrlBase->GetIListCtrlBase()->GetHWND(), &pt);
        ListItemBase*  pNowHover = m_pListCtrlBase->GetItemByWindowPoint(pt);
       
        if (!(lRet & IMKER_DONT_CHANGE_SELECTION))
        {
            if (m_pItemHover)  // 选中最后鼠标下的那一项，而不是鼠标当初按下的那项
            {
                if (m_pItemHover == pNowHover)
                {
                    // 只选这一个
                    // ListItemBase* pItemSave = m_pItemHover;
                    m_pListCtrlBase->SelectItem(m_pItemHover);
                }
            }
            else
            {
                m_pListCtrlBase->ClearSelectItem(true);
            }
        }

        if (!(lRet & (IMKER_HANDLED_BY_ITEM|IMKER_HANDLED_BY_INNERCONTROL)))
        {
            if (pSave &&  pSave == pNowHover)
            {
				UI::ListCtrlClickEventParam param = { 0 };
				param.pListCtrl = m_pListCtrlBase->GetIListCtrlBase();
				param.pHoverItem = pNowHover ? pNowHover->GetIListItemBase() : nullptr;
				param.pPressItem = pSave ? pSave->GetIListItemBase() : nullptr;

				m_pListCtrlBase->click.emit(&param);
            }
        }
    }
}
void  PopupListCtrlMKMgr::OnLButtonDown(UIMSG* pMsg)
{
    ListCtrlMKMgrBase::OnLButtonDown(pMsg);
}

void  PopupListCtrlMKMgr::OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg)
{
    if (pbInterestMsg)
        *pbInterestMsg = true;

    switch (pMsg->wParam)
    {
    case VK_DOWN:
        OnKeyDown_down(pMsg);
        return;

    case VK_UP:
        OnKeyDown_up(pMsg);
        return;

    default:
        __super::OnKeyDown(pMsg, pbInterestMsg);
        break;
    }
}

void  PopupListCtrlMKMgr::OnKeyDown_up(UIMSG* pMsg)
{
    if (nullptr == m_pItemHover)
    {
        ListItemBase* pSelItem = m_pListCtrlBase->GetFirstSelectItem();
        if (nullptr == pSelItem)
        {
            ListItemBase* pItem = m_pListCtrlBase->FindSelectableItemFrom(nullptr);
            if (pItem)
                m_pListCtrlBase->SelectItem(pItem);
        }
        else 
        {
            ListItemBase* pPrevItem = pSelItem->GetPrevSelectableItem();
            if (pPrevItem)
                m_pListCtrlBase->SelectItem(pPrevItem);
        }
    }
    else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
    {
        ListItemBase* pPrevItem = m_pItemHover->GetPrevSelectableItem();
        if (pPrevItem)
            m_pListCtrlBase->SelectItem(pPrevItem);
        else
            m_pListCtrlBase->SelectItem(m_pItemHover);
    }

    if (m_pListCtrlBase->GetFirstSelectItem())  // 清除hover对象，显示selection对象
    {
        SetHoverItem(nullptr);
    }
}

void  PopupListCtrlMKMgr::OnKeyDown_down(UIMSG* pMsg)
{
    ListItemBase* pSelItem = m_pListCtrlBase->GetFirstSelectItem();
    if (nullptr == m_pItemHover)
    {
        if (nullptr == pSelItem)
        {
            ListItemBase* pItem = m_pListCtrlBase->FindSelectableItemFrom(nullptr);
            if (pItem)
                m_pListCtrlBase->SelectItem(pItem);
        }
        else 
        {
            ListItemBase* pNextItem = pSelItem->GetNextSelectableItem();
            if (pNextItem)
                m_pListCtrlBase->SelectItem(pNextItem);		
        }
    }
    else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
    {
        ListItemBase* pNextItem = m_pItemHover->GetNextSelectableItem();
        if (pNextItem)
        {
            if (pSelItem == pNextItem)
                m_pListCtrlBase->InvalidateItem(pSelItem);
            else
                m_pListCtrlBase->SelectItem(pNextItem);
        }
        else
        {
            m_pListCtrlBase->SelectItem(m_pItemHover);
        }
    }

    if (m_pListCtrlBase->GetFirstSelectItem())  // 清除hover对象，显示selection对象
    {
        SetHoverItem(nullptr);
    }
}


