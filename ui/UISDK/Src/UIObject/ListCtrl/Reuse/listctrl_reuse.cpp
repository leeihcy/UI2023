#include "stdafx.h"
#include "listctrl_reuse.h"


ListCtrlReusable::ListCtrlReusable()
{

}

ListCtrlReusable::~ListCtrlReusable()
{

}

void ListCtrlReusable::Init(IListCtrlReusableDelegate* p)
{
	UIASSERT(p);
	m_pDelegate = p;

	if (m_pDelegate)
	{
		m_nRowCount = m_pDelegate->RowCount();
	}
}

void ListCtrlReusable::Release()
{
	m_pDelegate = nullptr;

	
}

void  ListCtrlReusable::OnPaint(UI::IRenderTarget*)
{

}

void ListCtrlReusable::makeView(LPCTSTR identify)
{

}