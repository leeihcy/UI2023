#pragma once

namespace UI
{
class ListItemBase;
class ListCtrlBase;

class ListCtrlInnerDragMgr
{
public:
    ListCtrlInnerDragMgr();
    ~ListCtrlInnerDragMgr();

    void  SetListCtrlBase(ListCtrlBase*  pCtrl);
	BOOL  ProcessMessage(UIMSG* pMsg);

	bool  IsDraging();
	void  CancelDrag();

    void  OnRemoveAll();
    void  OnRemoveItem(ListItemBase* pItem);

protected:
	BOOL  OnDragMouseMove(WPARAM wParam, LPARAM lParam);
	void  OnDragLButtonDown(WPARAM wParam, LPARAM lParam);
	BOOL  OnDragLButtonUp(WPARAM wParam, LPARAM lParam);

	void  StartDrag(POINT pt);
	void  AcceptDrag();
	void  Draging(POINT pt);
	void  __EndDrag();  // AcceptDrag/CancelDrag调用
   
protected:
    ListCtrlBase*   m_pListCtrlBase;

	ListItemBase*   m_pItemDrag;
	POINT			m_ptPress;
	int				m_nOldHScrollPos;
	int				m_nOldVScrollPos;
	CRect			m_rcOldPressItem;	// 拖拽项拖拽之前的rect
	int				m_nOldPressItemIndex;
	bool			m_bDraging;
	bool			m_bLButtondown;		// 不采用wParam & MK_LBUTTON
};

}