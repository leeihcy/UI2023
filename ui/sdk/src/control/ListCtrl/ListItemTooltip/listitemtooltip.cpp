#include "stdafx.h"
#include "listitemtooltip.h"
#include "../ListItemBase/listitembase.h"
#include "../listctrlbase.h"
#include "Inc/Interface/iuiinterface.h"

using namespace ui;

ListITemToolTipMgr::ListITemToolTipMgr()
{
    m_nActiveIndex = -1;
    m_pListItem = nullptr;
    m_pUIApp = nullptr;
    m_eMode = LITM_SINGLE;
}

ListITemToolTipMgr::~ListITemToolTipMgr()
{
    
}
void  ListITemToolTipMgr::SetListItem(ListItemBase* pItem)
{
    m_pListItem = pItem;

    IListCtrlBase*  pListCtrl = m_pListItem->GetIListCtrlBase();
    if (pListCtrl)
    {
        m_pUIApp = pListCtrl->GetUIApplication();
    }
}

bool  ListITemToolTipMgr::FindToolTip(long lId, __out_opt int* pIndex)
{
    size_t  nCount = m_vecToolTips.size();
    for (size_t i = 0; i < nCount ; i++)
    {
        if (m_vecToolTips[i].lId == lId)
        {
            if (pIndex)
                *pIndex = (int)i;

            return true;
        }
    }

    return false;
}

bool  ListITemToolTipMgr::AddToolTip(ListITemToolTipInfo* pInfo)
{
    if (!pInfo)
        return false;

    int lIndex = -1;
    if (this->FindToolTip(pInfo->lId, &lIndex))   
    {
        m_vecToolTips[lIndex] = *pInfo;
        return true;
    }

    m_vecToolTips.push_back(*pInfo);
    return true;
}

bool  ListITemToolTipMgr::RemoveToolTip(long lId)
{
    _MyIter iter = m_vecToolTips.begin();
    for (; iter != m_vecToolTips.end(); ++iter)
    {
        if (iter->lId == lId)
        {
            m_vecToolTips.erase(iter);
            return true;
        }
    }
    return false;
}

bool  ListITemToolTipMgr::ModifyToolTip(ListITemToolTipInfo* pInfo)
{
    if (!pInfo)
        return false;

    int nIndex = -1;
    if (!this->FindToolTip(pInfo->lId, &nIndex))   
        return false;

    m_vecToolTips[nIndex] = *pInfo;
    return true;
}

BOOL  ListITemToolTipMgr::ProcessMessage(UIMSG* pMsg, int nMsgMapId, bool bDoHook)
{
    if (pMsg->message == UI_MSG_GET_TOOLTIPINFO)
    {
        if (m_eMode == LITM_CUSTOM)
            return FALSE;

        pMsg->lRet = OnGetToolTipInfo(pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    else if (pMsg->message == WM_MOUSEMOVE)
    {
        if (m_eMode == LITM_MULTI)
        {
            OnMouseMove(pMsg->wParam, pMsg->lParam);
        }
    }
    else if (pMsg->message == WM_MOUSELEAVE)
    {
        if (m_eMode == LITM_MULTI)
        {
            OnMouseLeave();
        }
    }
    return FALSE;
}

long  ListITemToolTipMgr::OnGetToolTipInfo(WPARAM wParam, LPARAM lParam)
{
    IToolTipUI*  pToolTip = (IToolTipUI*)lParam;

    if (nullptr == pToolTip)
        return 0;

    if (0 == m_strToolTip.length())
        return 0;

    // 判断鼠标位置是否位于Item内，有可能是通过方向键响应的SetHoverItem来显示提示条，
    POINT  ptCursor;
    CRect  rcItem, rcWindowItem;

    ListCtrlBase*  pListCtrl = m_pListItem->GetListCtrlBase();
    if (!pListCtrl)
        return 0;

    ::GetCursorPos(&ptCursor);
	m_pListItem->GetVisibleRectInWindow(&rcWindowItem);

    MapWindowPoints(pListCtrl->GetIListCtrlBase()->GetHWND(), 
        nullptr, (LPPOINT)&rcWindowItem, 2);

    if (!rcWindowItem.PtInRect(ptCursor))
    {
        return 0;
    }

    if (m_eMode == LITM_SINGLE)
    {
        pToolTip->SetText(m_strToolTip.c_str());
        return 1;
    }
    else if (m_eMode == LITM_MULTI)
    {
        if (-1 != m_nActiveIndex)
        {
            if (m_vecToolTips[m_nActiveIndex].pTextRef)
            {
                pToolTip->SetText(m_vecToolTips[m_nActiveIndex].pTextRef->c_str());
                return 1;
            }
            else
            {
                // TODO: 发送消息
            }
        }
    }

    return 0;
}

void  ListITemToolTipMgr::SetToolTip(const wchar_t* szText)
{
    if (nullptr == szText)
    {
        m_strToolTip.clear();
        return;
    }
    m_strToolTip = szText;
}
const wchar_t*  ListITemToolTipMgr::GetToolTip()                
{
    return m_strToolTip.c_str(); 
}

void  ListITemToolTipMgr::ShowSingleToolTip()
{
    if (m_eMode == LITM_CUSTOM)
    {

    }
    else
    {
        if (m_eMode != LITM_SINGLE)
            return;

        if (m_strToolTip.empty())
            return;
    }

    if (!m_pUIApp)
        return;

    TOOLTIPITEM  item = {0};
    item.pNotifyObj = m_pListItem->GetIListItemBase();
    m_pUIApp->ShowToolTip(&item);
}

void  ListITemToolTipMgr::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    if (0 == m_vecToolTips.size())
        return;

    POINT  pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    ListCtrlBase*  pListCtrl = m_pListItem->GetListCtrlBase();
    if (!pListCtrl)
        return;

    pListCtrl->WindowPoint2ObjectClientPoint(&pt, &pt, true);
    int nHitTest = HitTest(pt);

    if (-1 == nHitTest)
    {
        if (m_nActiveIndex != -1)
        {
            m_pUIApp->HideToolTip();
            m_nActiveIndex = -1;
        }
    }
    else
    {
        if (m_nActiveIndex != nHitTest)
        {
            m_pUIApp->HideToolTip();

            m_nActiveIndex = nHitTest;

            TOOLTIPITEM  item = {0};
            item.pNotifyObj = m_pListItem->GetIListItemBase();
            m_pUIApp->ShowToolTip(&item);
        }
    }
}
void  ListITemToolTipMgr::OnMouseLeave()
{
    if (m_nActiveIndex != -1)    
    {
        m_nActiveIndex = -1;

        if (m_pUIApp)
            m_pUIApp->HideToolTip();
    }
}

int  ListITemToolTipMgr::HitTest(POINT ptClient)
{
    size_t  nCount = m_vecToolTips.size();
    for (size_t i = 0; i < nCount; i++)
    {
        if (PtInRect(&m_vecToolTips[i].region, ptClient))
            return (int)i;
    }
    return -1;
}

void  ListITemToolTipMgr::SetToolTipMode(LISTITEMTOOLTIPMODE eMode)
{
    m_eMode = eMode;
}