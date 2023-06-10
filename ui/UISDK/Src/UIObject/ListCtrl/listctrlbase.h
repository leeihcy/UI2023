 #pragma once
#include "Inc\Interface\ilistctrlbase.h"
#include "Src\UIObject\Control\control.h"
#include "Src\UIObject\ScrollBar\scrollbarmanager.h"
#include "ListItemLayout\listitemlayout.h"
#include "MouseKeyboard\listctrlmkmgr.h"
#include "MouseKeyboard\listctrl_inner_drag.h"
#include "float/float_item_mgr.h"
#include "Reuse/listctrl_reuse.h"

namespace UI
{
class ListCtrlBase;
interface IListItemBase;
class CustomListItem;

//////////////////////////////////////////////////////////////////////////
// 内部消息

// 延迟删除
// message: UI_MSG_NOTIFY
// code:    UI_LCN_INNER_DELAY_REMOVE_ITEM  
// wparam:  ListCtrlBase*
// lParam:  int nFlags
#define UI_LCN_INNER_DELAY_REMOVE_ITEM  138072146

class ListItemShareData : public Message
{
public:
    ListItemShareData(IListItemShareData* p);
    ~ListItemShareData();

	UI_BEGIN_MSG_MAP()
    UI_END_MSG_MAP()

    void  SetListCtrlBase(ListCtrlBase* p);
    IListCtrlBase*  GetListCtrlBase();

private:
    IListItemShareData*  m_pIListItemShareData;
    ListCtrlBase*  m_pListCtrlBase;
};

// 列表控件基类
class  ListCtrlBase : public Control
{
public:
	ListCtrlBase(IListCtrlBase* p);
	~ListCtrlBase();

	UI_BEGIN_MSG_MAP()
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
		if (m_pInnerDragMgr)
		{
			if (m_pInnerDragMgr->ProcessMessage(pMsg))
			{
				return TRUE;
			}
		}
        if (m_pMKMgr)
        {
            if (m_pMKMgr->ProcessMessage(pMsg))
            {
                return TRUE;
            }
        }
        UIMSG_HANDLER_EX(UI_LCM_HITTEST, OnHitTest)
		UIMSG_PAINT(OnPaint)
        UIMSG_POSTPAINT(OnPostPaint)
        UIMSG_HANDLER_EX(UI_MSG_INERTIAVSCROLL, OnInertiaVScroll)
		UIMSG_STATECHANGED(OnStateChanged)
		UIMSG_SIZE(OnSize) 
		UIMSG_VSCROLL(OnVScroll)
        UIMSG_HSCROLL(OnHScroll)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
		UIMSG_HANDLER_EX(UI_WM_GESTURE_PAN, OnGesturePan)
		UICHAIN_MSG_MAP_MEMBER(m_mgrScrollBar)
        UIMSG_NOTIFY(UI_LCN_INNER_DELAY_REMOVE_ITEM, OnDelayRemoveItem)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
		UIMSG_SERIALIZE(OnSerialize)
        UIMSG_FINALCONSTRUCT(FinalConstruct)            
        UIMSG_FINALRELEASE(FinalRelease)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

    IListCtrlBase*  GetIListCtrlBase();

protected:
	// 消息处理
    HRESULT  FinalConstruct(ISkinRes* p);
    void  FinalRelease();
	void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnPostPaint(IRenderTarget* pRenderTarget);
	void  OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
    void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnSize( UINT nType, int cx, int cy );
	void  OnStateChanged(UINT nMask);
	void  OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    void  OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    LRESULT  OnInertiaVScroll(
				UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  GetDesiredSize(SIZE* pSize);
	void  OnSerialize(SERIALIZEDATA* pData);
    LRESULT  OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
    HANDLED_VALUE  OnDrawItem(
				IRenderTarget* pRenderTarget, ListItemBase* p);
    LRESULT  OnDelayRemoveItem(WPARAM w, LPARAM l);
    LRESULT  OnGesturePan(UINT, WPARAM, LPARAM);

public:
	// 公用接口
    bool  AddItem(ListItemBase* pItem);
    bool  InsertItem(
            ListItemBase* pItem, 
            UINT nPos);
    bool  InsertItem(
            ListItemBase* pItem, 
            ListItemBase* pInsertAfter);
    bool  InsertItem(
            ListItemBase* pItem, 
            IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* pInsertAfter = UITVI_LAST);

    void  RemoveItem(ListItemBase* pItem);
	void  RemoveItem(int nIndex);
    void  RemoveAllChildItems(ListItemBase* pParent);
	void  RemoveAllItem();
    void  DelayRemoveItem(ListItemBase* pItem);
    void  ItemDestructed(ListItemBase* pItem);

	void  SetSortCompareProc(ListItemCompareProc p);
    void  Sort();
    void  SortChildren(ListItemBase* pParent);
    void  SwapItemPos(ListItemBase*  p1, ListItemBase* p2);
    void  RemoveItemFromTree(ListItemBase*  p);
    bool  MoveItem(
            ListItemBase*  p, 
            IListItemBase* pNewParent, 
            IListItemBase* pInsertAfter);

	int   GetItemCount() { return m_nItemCount; }
    int   GetVisibleItemCount();
    int   GetRootItemCount();
	LPCTSTR   GetItemText(ListItemBase* pItem);
	
	ListItemBase*  GetItemByPos(UINT i, bool bVisibleOnly=true);
    int  GetItemPos(ListItemBase*, bool bVisibleOnly = true);
    ListItemBase*  GetItemByWindowPoint(POINT pt);
    ListItemBase*  GetItemUnderCursor();
    ListItemBase*  GetItemById(long lId);
    ListItemBase*  FindItemByText(
            LPCTSTR  szText, 
            ListItemBase* pStart=nullptr);
    ListItemBase*  FindChildItemByText(
            LPCTSTR  szText, 
            ListItemBase* pParent=nullptr, 
            ListItemBase* pStart=nullptr);
	ListItemBase*  GetFirstItem();
	ListItemBase*  GetLastItem();
	ListItemBase*  GetFirstDrawItem();
	ListItemBase*  GetLastDrawItem();
	ListItemBase*  GetEditingItem();
	void  SetEditingItem(ListItemBase* p);
    ListItemBase*  FindVisibleItemFrom(ListItemBase* pFindFrom=nullptr);
    ListItemBase*  FindSelectableItemFrom(
            ListItemBase* pFindFrom=nullptr);
    ListItemBase*  FindFocusableItemFrom(
            ListItemBase* pFindFrom=nullptr);
    ListItemBase*  EnumItemByProc(
            ListItemEnumProc pProc, 
            ListItemBase* pEnumFrom = nullptr, 
            WPARAM w = 0, 
            LPARAM l = 0);
    void  ItemIdChanged(
            IListItemBase* pItem, 
			UINT_PTR lOldId,
			UINT_PTR lNewId);

	void  SetLayout(IListCtrlLayout* pLayout);
    void  SetLayoutFixedHeight();
    void  SetLayoutVariableHeight();
	IListCtrlItemFixHeightFlowLayout*  SetLayoutFixedHeightFlow();
    IListCtrlLayout*  GetLayout();
	void  EnableInnerDrag(bool b);
	void  EnableReusable(IListCtrlReusableDelegate* p);

	SIZE  GetAdaptWidthHeight(int nWidth, int nHeight);

	short  GetVertSpacing() { 
			return m_nVertSpacing; }
	short  GetHorzSpacing() { 
			return m_nHorzSpacing; }
	int   GetItemHeight() { 
			return m_nItemHeight;}
	void  SetItemHeight(int nHeight, bool bUpdate=true);
    int   GetChildNodeIndent();
    void  SetChildNodeIndent(int n);
	void  GetItemContentPadding(REGION4* prc) {
            ::CopyRect(prc, &m_rItemContentPadding); }
	void  SetItemContentPadding(REGION4* prc) { 
            ::CopyRect(&m_rItemContentPadding, prc); }

    void  CalcFirstLastDrawItem();
    void  SetCalcFirstLastDrawItemFlag();

    void  UpdateListIfNeed();
    void  SetNeedLayoutItems();
	void  SetNeedSortItems();
	void  LayoutItem(ListItemBase* pStart, bool bRedraw);
    void  UpdateItemIndex(ListItemBase* pStart);
    IScrollBarManager*  GetIScrollBarMgr();
    FloatItemMgr&  GetFloatItemMgr() { return m_MgrFloatItem; }

    void  ToggleItemExpand(ListItemBase* pItem, bool bUpdate);
    void  CollapseItem(ListItemBase* pItem, bool bUpdate);
    void  ExpandItem(ListItemBase* pItem, bool bUpdate);
    void  CollapseAll(bool bUpdate);
    void  ExpandAll(bool bUpdate);

	// Custom List Item
	CustomListItem*  InsertCustomItem(
			LPCTSTR  szLayoutName,
			long lId,
			IListItemBase* pParent = UITVI_ROOT, 
			IListItemBase* pInsertAfter = UITVI_LAST);
	CustomListItem*  GetCustomListItem(Object* p);

	// InvalidateItem 操作
	void  InvalidateItem(ListItemBase* pItem);
	void  RedrawItemByInnerCtrl(
            IRenderTarget* pRenderTarget, 
            ListItemBase* pItem);
    void  MakeItemVisible(ListItemBase* pItem, bool* pbNeedUpdate);

    IRenderBase*  GetFocusRender() { return m_pFocusRender; }
    void  SetFocusRender(IRenderBase* p);

	// select 操作
    void  SelectItem(
            ListItemBase* pItem, 
            bool bNotify=true, 
            bool bMakeVisible=true);
    void  AddSelectItem(ListItemBase* pItem, bool bNotify);
	void  RemoveSelectItem(ListItemBase* pItem, bool bNotify);
	void  ClearSelectItem(bool bNotify);
	bool  IsSelected(ListItemBase* pItem);
	ListItemBase*  GetFirstSelectItem() { 
            return m_pFirstSelectedItem; }
	ListItemBase*  GetLastSelectItem();
    UINT  GetSelectedItemCount();
    bool  IsSelectMulti();
    void  SelectAll(bool bUpdate);

    void  SetHoverItem(ListItemBase* pItem);
    void  SetPressItem(ListItemBase* pItem);
	ListItemBase*  GetHoverItem();
	ListItemBase*  GetPressItem();
	ListItemBase*  GetFocusItem();
	void  SetFocusItem(ListItemBase* pItem);
    Object*  GetHoverObject();
    Object*  GetPressObject();
    void  SetFocusObject(Object* pObj);
    Object*  GetFocusObject();
//	void  CancelEdit();

    // item share data op
    IListItemShareData*  GetItemTypeShareData(int lType);
    void  SetItemTypeShareData(
            int lType, 
            IListItemShareData* pData);
    void  RemoveItemTypeShareData(int lType);
    
    void  ObjectRect2WindowRect(RECT* prcObj, RECT* prcWindow);
	void  ObjectRect2ItemRect(RECT* prcObject, RECT* prcItem);
    void  ItemRect2ObjectRect(RECT* prcItem, RECT* prcObject);
	void  ItemRect2WindowRect(LPCRECT prc, LPRECT prcRet);
    void  WindowPoint2ItemPoint(
            ListItemBase* pItem, 
            const POINT* pt, 
            POINT* ptRet);
    BOOL  ProcessItemMKMessage(UIMSG* pMsg, ListItemBase* pItem);

    void  FireSelectItemChanged(ListItemBase* pOldValue);
    void  ListItemDragDropEvent(
            UI::DROPTARGETEVENT_TYPE eEvent, 
            ListItemBase* pItem);
    void  ListCtrlDragScroll();

    bool  Scroll2Y(int nY, bool bUpdate);
    bool  Scroll2X(int nX, bool bUpdate);
    bool  ScrollY(int nY, bool bUpdate);
    bool  ScrollX(int nX, bool bUpdate);
	bool  SetScrollPos(int nX, int nY, bool bUpdate);
	void  GetScrollPos(int* pnX, int* pnY);

	void  ModifyListCtrlStyle(
            LISTCTRLSTYLE* add, 
            LISTCTRLSTYLE* remove);
	bool  TestListCtrlStyle(LISTCTRLSTYLE* test);

    bool  IsItemVisibleInScreen(ListItemBase* pItem);
    bool  IsItemRectVisibleInScreen(LPCRECT prc);
    bool  IsItemVisibleInScreenEx(
            ListItemBase* pItem, 
            /*LISTITEM_VISIBLE_POS_TYPE*/long& ePos);

	ListCtrlMKMgrBase*  GetMKManager() { return m_pMKMgr; }

protected:
    bool  _InsertItemToTree(
            ListItemBase*  p, 
            IListItemBase* pParent, 
            IListItemBase* pAfter);
	bool  _InsertItem(
            ListItemBase* pItem, 
            ListItemBase* pInsertAfter);
    bool  _InsertLastChild(
            ListItemBase* pNewItem, 
            ListItemBase* pParent);
    bool  _InsertFirstChild(
            ListItemBase* pNewItem, 
            ListItemBase* pParent);
    bool  _InsertBefore(
            ListItemBase* pNewItem, 
            ListItemBase* pInsertFront);
    bool  _InsertAfter(
            ListItemBase* pNewItem, 
            ListItemBase* pInsertAfter);
    bool  _InsertRoot(ListItemBase* pNewItem);

	bool  _RemoveItem(ListItemBase* pItem, bool bReleaseItem);
    void  _RemoveItemFromTree(ListItemBase*  p);
	bool  _RemoveAllItem();
    bool  _RemoveAllChildItems(ListItemBase* pParent);
	void  _SendItemRemoveNotify(ListItemBase* pItem);

    bool  is_my_item(IListItemBase* pItem);
    ListItemBase*  get_valid_list_item(IListItemBase* pItem);

    void  sort_by_first_item(
            ListItemBase* pFirstItem, 
            int nNeighbourCount, 
            bool bSortChildren);
	void  arrange_item(ListItemBase* pFrom, __out SIZE* pContent);
    void  update_mouse_mgr_type();

protected:
	virtual IMKMgr* virtualGetIMKMgr() override;

protected:
	friend class  ListCtrlMouseMgr;
	friend class  ListItemBase;

    IListCtrlBase*  m_pIListCtrlBase;

    ListCtrlMKMgrBase*    m_pMKMgr;
	ListCtrlInnerDragMgr*  m_pInnerDragMgr;

    ListCtrlLayoutMgr     m_MgrLayout;      // 布局对象
    ScrollBarManager      m_mgrScrollBar;   // 滚动条
    FloatItemMgr          m_MgrFloatItem;   // 浮动元素
	ListCtrlReusable*     m_pReusable;      // 列表重用

	// 数据导航
	ListItemBase*  m_pFirstItem;
	ListItemBase*  m_pLastItem;   // 树型控件，lastitem指lastrootitem
	ListItemBase*  m_pFirstDrawItem;   
	ListItemBase*  m_pLastDrawItem;   
	ListItemBase*  m_pFirstSelectedItem;
    ListItemBase*  m_pEditingItem; // 正在编辑的对象

    // 不要使用win32edit，尽量使用uiedit。win32edit有以下缺陷：
    // 1. 创建时要抢焦点，刷新时序不好控制
    // 2. 方形控件，无法实现圆角背景。
    // 3. popup弹出式窗口，如果出现在layered窗口上，会导致闪烁
    // TODO: IEdit*  m_pEdit;  // 编辑控件

    // 属性
	LISTCTRLSTYLE  m_listctrlStyle;

    // 列表项的内容间距。例如用于实现文本左缩进等
    CRegion4  m_rItemContentPadding;          

    short  m_nVertSpacing;                   // 行间距
    short  m_nHorzSpacing;                   // 横向间距
    int    m_nChildNodeIndent;               // 子结点缩进
    UINT   m_nItemCount;                     // 列表项数量
    long   m_nItemHeight;                    // 列表项的高度

    // auto size列表框的最小尺寸，包括nonclient region。
    // 它不是指content size，而是控件大小
    // SIZE   m_sizeMin;                        

    // auto size列表框的最大尺寸。但是在配置了 width/height
    // 属性时，m_sizeMin/m_sizeMax可能无效了
    // SIZE   m_sizeMax;                        

    // 标识重新计算需要绘制的对象
    bool   m_bNeedCalcFirstLastDrawItem;  
    // 标识需要重新布局所有item位置
    bool   m_bNeedLayoutItems;
    // 标识需要重新计算items索引        
    bool   m_bNeedUpdateItemIndex;
    // 标识需要重新排序 
    bool   m_bNeedSortItems;

    // 正在刷新无效项。用于在OnPaint中判断刷新哪些
    // bool   m_bRedrawInvalidItems;            

    // 控件正在刷新。期间不允许再触发Refresh操作，
    // 防止同一堆栈上有多次刷新操作，导致刷新状态混乱
    byte   m_bPaintingCtrlRef;               

    // 排序函数
    ListItemCompareProc   m_pCompareProc;    

    // item扩展，为每一个类型的item提供一个保存共享数据的方法
    map<int, IListItemShareData*>  m_mapItemTypeShareData;  

    // 用于快速定位
    typedef  map<UINT_PTR, IListItemBase*>::iterator _mapItemIter;
    map<UINT_PTR, IListItemBase*>  m_mapItem;

    // 绘制
    IRenderBase*   m_pFocusRender;

public:
	// event
	signal<IListCtrlBase*>  select_changed;
	signal<ListCtrlClickEventParam*>  click;

#if _MSC_VER >= 1800
    signal<IListCtrlBase*, IListItemBase*>  rclick;
    signal<IListCtrlBase*, IListItemBase*>  mclick;
    signal<IListCtrlBase*, IListItemBase*>  dbclick;

	// 键盘消息 vKey / bool是否已处理
	signal<IListCtrlBase*, UINT, bool&>  keydown;
#else
	signal2<IListCtrlBase*, IListItemBase*>  rclick;
	signal2<IListCtrlBase*, IListItemBase*>  mclick;
	signal2<IListCtrlBase*, IListItemBase*>  dbclick;

	// 键盘消息 vKey / bool是否已处理
	signal3<IListCtrlBase*, UINT, bool&>  keydown;
#endif
    
};
}