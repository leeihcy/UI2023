#pragma once
#include "include/interface/ilistctrlbase.h"

namespace ui
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

	void  makeView(const wchar_t* identify);

	//visibleCells[]
	//reusableCells[]

private:
	IListCtrlReusableDelegate*  m_pDelegate = nullptr;
	int m_nRowCount = 0;
};


}