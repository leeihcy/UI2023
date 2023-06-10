#include "stdafx.h"
#include "custom_listitem.h"
#include "Inc\interface\ipanel.h"
#include "..\listctrlbase.h"

CustomListItem::CustomListItem(ICustomListItem* p) : ListItemBase(p)
{
	m_pICustomListItem = p;
	p->SetItemFlag(ICustomListItem::FLAG);
}
CustomListItem::~CustomListItem()
{

}

IObject*  CustomListItem::FindControl(LPCTSTR szId)
{
	if (!m_pPanelRoot)
		return nullptr;

	return m_pPanelRoot->FindObject(szId);
}


void  CustomListItem::OnPaint(IRenderTarget* pRenderTarget)
{
	UIASSERT (m_pListCtrlBase);

	CRect  rcParent;
	this->GetParentOrFloatRect(&rcParent);

	UINT nRenderState = 0;
	IRenderBase* pRender = m_pListCtrlBase->GetForeRender();
	if (pRender)
	{
		nRenderState = m_pICustomListItem->GetItemRenderState();
		pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
	}
}



void CustomListItem::SetStringData(LPCSTR data)
{
	if (data)
		m_strData = data;
	else
		m_strData.clear();
}

LPCSTR CustomListItem::GetStringData()
{
	return m_strData.c_str();
}

