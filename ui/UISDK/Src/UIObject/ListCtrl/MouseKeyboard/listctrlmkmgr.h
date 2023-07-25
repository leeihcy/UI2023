#pragma once
#include "Inc\private_inc.h"

namespace ui
{
class Object;
class ListCtrlBase;
class ListItemBase;
class UIApplication;

class ListCtrlMKMgrBase : public IMKMgr
{
public:
    ListCtrlMKMgrBase();
    virtual ~ListCtrlMKMgrBase();

    BOOL ProcessMessage(UIMSG* pMsg)
    {
        return DoProcessMessage(pMsg);
    }
    BOOL ProcessItemMessage(UIMSG* pMsg, ListItemBase* pItem)
    {
        return DoProcessItemMessage(pMsg, pItem);
    }
    void  SetListCtrlBase(UIApplication* pUIApp, ListCtrlBase*  pCtrl);

    void  OnRemoveAll();
    void  OnRemoveItem(ListItemBase* pItem, bool* pbSelChanged);
	void  OnHideItem(ListItemBase* pItem);
	void  OnDisableItem(ListItemBase* pItem);

    ListItemBase*  GetPressItem() 
    {
        return m_pItemPress;
    }
    ListItemBase*  GetHoverItem()
    {
        return m_pItemHover;
    }
    ListItemBase*  GetFocusItem()
    {
        return  m_pItemFocus;
    }
    Object*  GetHoverObject()
    {
        return m_pObjHover;
    }
    Object*  GetPressObject()
    {
        return m_pObjPress;
    }
    Object*  GetFocusObject()
    {
        return m_pObjFocus;
    }
    void  SetMouseNotReady(){
        m_bMouseMoveReady = false;
    }

    void  SetHoverItem(ListItemBase* pItem);
    void  SetPressItem(ListItemBase* pItem);
    void  SetFocusItem(ListItemBase* pItem);

    void  SetHoverObject(Object* pObj);
    void  SetPressObject(Object* pObj);
    void  SetFocusObject(Object* pObj);

    ListItemBase*  GetItemByPos(POINT ptWindow);
protected:
    virtual BOOL  DoProcessMessage(UIMSG* pMsg);
    virtual BOOL  DoProcessItemMessage(UIMSG* pMsg, ListItemBase* pItem);
    
    virtual void  OnObjectVisibleChangeInd(Object* pObj, bool bVisible) override;
    virtual void  OnObjectRemoveInd(Object* pObj) override;

protected:
    void  OnMouseMove(UIMSG* pMsg);
    void  OnMouseLeave(UIMSG* pMsg);
	LONG_PTR  OnLButtonDown(UIMSG* pMsg);
    void  OnLButtonUp(UIMSG* pMsg);
    void  OnLButtonDBClick(UIMSG* pMsg);
    void  OnRButtonDown(UIMSG* pMsg);
    void  OnRButtonUp(UIMSG* pMsg);
    void  OnMButtonDBClick(UIMSG* pMsg);
    void  OnMButtonDown(UIMSG* pMsg);
    void  OnMButtonUp(UIMSG* pMsg);
    void  OnSetFocus(UIMSG* pMsg);
    void  OnKillFocus(UIMSG* pMsg);
    bool  OnSetCursor(UIMSG* pMsg);
	BOOL  OnGestureMessage(UIMSG* pMsg);
    bool  OnKeyMsg(UIMSG* pMsg);

    void  OnItemMouseMove(UIMSG* pMsg, ListItemBase* pItem);
    void  OnItemMouseLeave(UIMSG* pMsg, ListItemBase* pItem);
    void  OnItemLButtonDown(UIMSG* pMsg, ListItemBase* pItem);
    void  OnItemLButtonDBClick(UIMSG* pMsg, ListItemBase* pItem, BOOL* pbHandled);
    void  OnItemLButtonUp(UIMSG* pMsg, ListItemBase* pItem);
    void  OnItemKillFocus(UIMSG* pMsg, ListItemBase* pItem);
    bool  OnItemSetCursor(UIMSG* pMsg, ListItemBase* pItem);
    bool  OnItemKeyMsg(UIMSG* pMsg, ListItemBase* pItem);

    void  FireHoverItemChanged(ListItemBase*  pOldValue);
	
	enum ITEM_STATE_CHANGE_TYPE
	{
		ITEM_REMOVED,
		ITEM_HIDE,
		ITEM_DISABLED
	};
	void  on_item_state_changed(
			ListItemBase* pItem,
			ITEM_STATE_CHANGE_TYPE eType, 
			bool* pbSelChanged);

protected:
    Object*   m_pObjHover;
    Object*   m_pObjPress;
    Object*   m_pObjFocus;

    ListItemBase*   m_pItemHover;
    ListItemBase*   m_pItemPress;
    ListItemBase*   m_pItemFocus;
    ListItemBase*   m_pItemRPress;
    ListItemBase*   m_pItemMPress;
//    ListItemBase*   m_pItemSelected;  -- 历史原因，该字体暂时仍然放在listctrl中进行维护
//                                      -- 等mk改造完成之后再进行调整。
	bool  m_bKeepHoverWhenPress;  // 当按下时，鼠标移动不切换hoveritem。默认为true，特殊值如Menu为false
    bool  m_bMouseMoveReady;

    UIApplication*  m_pUIApplication;
    ListCtrlBase*   m_pListCtrlBase;

    template<class T>
    friend void _SetHoverObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _SetPressObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _OnMouseMove(Object* pObj, WPARAM wParam, LPARAM lParam, T* pThis);
    template <class T>
    friend void _OnMouseLeave(T* pThis);
    template <class T>
    friend void _OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonUp(WPARAM w, LPARAM l, T* pThis);
};

class SingleSelListCtrlMKMgr : public ListCtrlMKMgrBase
{
protected:
    virtual BOOL  DoProcessMessage(UIMSG* pMsg);

protected:
    void  OnLButtonDown(UIMSG* pMsg);
    void  OnLButtonUp(UIMSG* pMsg);
    void  OnRButtonDown(UIMSG* pMsg);
    void  OnRButtonUp(UIMSG* pMsg);

    void  OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg);
    void  OnKeyDown_down(UIMSG* pMsg);
    void  OnKeyDown_up(UIMSG* pMsg);
	void  OnKeyDown_space(UIMSG* pMsg);
    void  OnKeyDown_prior(UIMSG* pMsg);
    void  OnKeyDown_next(UIMSG* pMsg);
    void  OnKeyDown_home(UIMSG* pMsg);
    void  OnKeyDown_end(UIMSG* pMsg);

    void  handle_selection_change();
};

}