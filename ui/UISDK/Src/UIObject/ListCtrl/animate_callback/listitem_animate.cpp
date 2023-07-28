#include "stdafx.h"
#include "listitem_animate.h"
#include "include/interface/ilistctrlbase.h"


ListItemAnimateImpl::ListItemAnimateImpl()
{
    m_pListItem = nullptr;

	//m_funEndCallback = nullptr;
	//m_pEndCallbackData = nullptr;
}
ListItemAnimateImpl::~ListItemAnimateImpl()
{
    if (m_pListItem)
    {
        m_pListItem->RemoveDelayRef((void**)&m_pListItem);
    }
}

bool  ListItemAnimateImpl::PreStart()
{
    if (!__super::PreStart())
        return false;

    if (!m_pListItem)
        return false;

    m_pListItem->AddDelayRef((void**)&m_pListItem);
    return true;
}


void  ListItemAnimateImpl::Bind(UI::IListItemBase& item)
{
    m_pListItem = &item;
}

UI::IListCtrlBase* ListItemAnimateImpl::GetListCtrl()
{
    if (!m_pListItem)
        return nullptr;

    return m_pListItem->GetIListCtrlBase();
}

bool  ListItemAnimateImpl::IsEqual(ListItemAnimateImpl* p)
{
    if (p->GetUniquieId() != GetUniquieId())
        return false;

    if (p->m_pListItem != m_pListItem)
        return false;

    return true;
}

UI::IApplication*  ListItemAnimateImpl::GetUIApplication()
{
    if (GetListCtrl())
        return GetListCtrl()->GetUIApplication();
    return nullptr;
}

void  ListItemAnimateImpl::PostEnd()
{
// 	if (m_funEndCallback)
// 	{
// 		m_funEndCallback(m_pListItem, m_pEndCallbackData);
// 	}
}


// void  ListItemAnimateImpl::SetEndCallback(pfnListItemAnimateEndCallback f, long* pData)
// {
//	m_funEndCallback = f;
//	m_pEndCallbackData = pData;
//}