#pragma once
#include "Inc\Interface\ilistctrlbase.h"

namespace UI
{


class ListCtrlReusable
{
public:
	ListCtrlReusable();
	~ListCtrlReusable();

	// reloadData
	// reloadRowsAtIndex

	void  Init(IListCtrlReusableDelegate* p);
	void  Release();

	void  OnPaint(UI::IRenderTarget*);

	void  makeView(LPCTSTR identify);

	//visibleCells[]
	//reusableCells[]

private:
	IListCtrlReusableDelegate*  m_pDelegate = nullptr;
	int m_nRowCount = 0;
};


}