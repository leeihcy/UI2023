#ifndef _UI_LISTCTRLBASE_H_
#define _UI_LISTCTRLBASE_H_
#include "icontrol.h"
#include "..\Util\struct.h"

namespace ui
{
    class ListCtrlBase;
    interface IListItemBase;
	interface IListCtrlBase;
	interface ICustomListItem;
    interface IScrollBarManager;
    interface IListCtrlLayout;
	interface IListCtrlItemFixHeightFlowLayout;
    interface IListItemShareData;
	interface IListCtrlReusableDelegate;
    class RenderContext;
}

//////////////////////////////////////////////////////////////////////////
// ListCtrl Notify/Message
//////////////////////////////////////////////////////////////////////////

namespace ui
{
    struct  LISTCTRL_CLICKNOTIFY_DATA
    {
        IListItemBase*  pDowmItem;  // 鼠标按下时的item，有可能弹起时不在该item上，这由外部去判断。
        WPARAM  wParam;
        LPARAM  lParam;
    };

	struct ListCtrlClickEventParam
	{
		IListCtrlBase*  pListCtrl;
		IListItemBase*  pHoverItem;
		IListItemBase*  pPressItem;
	};
}

//	双击listctrl
//		message: UI_MSG_NOTIFY
//		code:    UI_LCN_DBCLICK
//		wparam:  LISTCTRL_CLICKNOTIFY_DATA*
// #define UI_NM_DBCLICK   135311303

//	点击listctrl
//		message: UI_MSG_NOTIFY
//		code:    UI_LCN_CLICK
//		wparam:  LISTCTRL_CLICKNOTIFY_DATA*
//#define UI_NM_CLICK     135311304

//  右击listctrl
//		message: UI_MSG_NOTIFY
//		code:    UI_NM_RCLICK
//		wparam:  LISTCTRL_CLICKNOTIFY_DATA*
// #define UI_NM_RCLICK     136291126

// 滚轮点击listctrl
//      message: UI_MSG_NOTIFY
//      code:    UI_NM_MCLICK
//      wParam: LISTCTRL_CLICKNOTIFY_DATA*
//#define  UI_NM_MCLICK    158141138
 
//  键盘消息
//      message: UI_MSG_NOTIFY
//      code:    UI_NM_KEYDOWN
//      wParam:  vKey
//      lParam:  
//      return:  1表示已处理，0表示未处理
// #define UI_NM_KEYDOWN    140081548

//	listctrl 当前选中项改变(combobox)
//		message: UI_MSG_NOTIFY
//		code:    UI_LCN_SELCHANGED
//		wparam:  IListItemBase* pOld
//		lparam:  IListItemBase* pNew
#define UI_LCN_SELCHANGED_SELF  135311306   // 给自己派生类发送的
// #define UI_LCN_SELCHANGED  135311305  // 给自己发送的。废弃，修改为signal


//  listctrl当前hover项改变\
//		message: UI_MSG_NOTIFY
//		code:    UI_LCN_HOVERITEMCHANGED
//		wparam:  IListItemBase* pOld
//		lparam:  IListItemBase* pNew    
#define UI_LCN_HOVERITEMCHANGED 135311306

// DrawItem
//      wParam: IRenderTarget*
//      lParam: IListItemBase*
#define UI_LCN_DRAWITEM 135311307

// 检测一个坐标位于哪个可见item上
// HitTest
//     wParam: x (控件坐标，已计算了偏移，滚动等)
//     lParam: y
//     ret: IListItemBase*
//
#define UI_LCM_HITTEST  136041628

//
// 通知listctrl派生类添加了一个ITEM
// Message: UI_MSG_NOTIFY
// Code:    UI_LCN_REMOVEITEM
// WPARAM:  IListItemBase*
// LPARAM:  
//
#define UI_LCN_ITEMADD      136131220

// 通知listctrl派生类正在删除一个ITEM
// Message: UI_MSG_NOTIFY
// Code:    UI_LCN_PRE_ITEMREMOVE
// WPARAM:  IListItemBase*
#define UI_LCN_PRE_ITEMREMOVE    136131221

// 通知listctrl派生类正在删除一个ITEM
// Message: UI_MSG_NOTIFY
// Code:    UI_LCN_PRE_ITEMREMOVE
// WPARAM:  long lItemId
// LPARAM:  IListItemBase* (通知完之后，该item将被删除)
#define UI_LCN_ITEMREMOVE    136131632

// 通知listctrl派生类正在删除所有ITEM
// Message: UI_MSG_NOTIFY
// Code:    UI_LCN_REMOVEALLITEM
#define UI_LCN_PRE_ALLITEMREMOVE 136131222

// 通知listctrl派生类正在删除所有ITEM
// Message: UI_MSG_NOTIFY
// Code:    UI_LCN_REMOVEALLITEM
#define UI_LCN_ALLITEMREMOVE 136131223

// 数据懒加载
// wParam: IListItemBase*
#define UI_LCN_LAZYLOADDATA  156301212

// first/last draw item更新(需要float_group_head样式)
// 发送给列表派生类
#define UI_LCN_FIRSTLASTDRAWITEM_UPDATED  157021559

// 通知子类立即结束编辑状态。listctrl本身不提供编辑功能。但有些通知只能从listitembase中
// 发出来，如item在编辑过程中被销毁了
#define UI_MSG_LISTCTRL_DISCARDEDIT  171131553

#define UITVI_ROOT    ((UI::IListItemBase*)TVI_ROOT)
#define UITVI_FIRST   ((UI::IListItemBase*)TVI_FIRST)
#define UITVI_LAST    ((UI::IListItemBase*)TVI_LAST)

namespace ui
{

typedef bool (*ListItemCompareProc)(IListItemBase* p1, IListItemBase* p2);
typedef bool (*ListItemEnumProc)(IListItemBase* pItem, WPARAM, LPARAM);   // 返回false时停止enum

enum LISTITEM_VISIBLE_POS_TYPE
{
    LISTITEM_UNVISIBLE_ERROR = 0,
    LISTITEM_VISIBLE = 0x10000000,

    //--------------------------
    LISTITEM_UNVISIBLE_TOP = 0x01,      
    LISTITEM_VISIBLE_COVERTOP = 0x02,       
    LISTITEM_VISIBLE_COVERBOTTOM = 0x04,
    LISTITEM_UNVISIBLE_BOTTOM = 0x08,
    //--------------------------
    LISTITEM_UNVISIBLE_LEFT = 0x10,
    LISTITEM_VISIBLE_COVERLEFT = 0x20,
    LISTITEM_VISIBLE_COVERRIGHT = 0x40,
    LISTITEM_UNVISIBLE_RIGHT = 0x80,
};

#if 0
// 添加列表项的一些标识，主要是用于提高批量添加时的效率
enum LISTCTRL_OP_FLAG
{
	LF_NONE =             0x0000,
	// LF_REDRAW =           0x0001,    // 刷新 过期，统一采用延迟刷新，外面不用管刷新逻辑
	LF_LAYOUT =           0x0002,    // 计算每个子项的位置
	LF_SORT =             0x0004,    // 排序。最后必须调用一次
	LF_UPDATEITEMINDEX =  0x0008,    // 更新索引
	LF_ALL =  (/*LF_REDRAW|*/LF_LAYOUT|LF_SORT|LF_UPDATEITEMINDEX),
};
typedef long LISTITEM_OPFLAGS;
#endif

typedef struct tagListCtrlStyle
{
	bool  sort_ascend : 1;        // 升序排序
	bool  sort_descend : 1;       // 降序排序
	bool  sort_child : 1; 
	bool  multiple_sel : 1;       // 是否支持多选
	bool  popuplistbox : 1;       // 弹出式listbox式鼠标管理器，1.鼠标按下时不选择，鼠标弹起时选择 2. 当没有hover时显示sel 3. 当有hover时显示hover 4. 鼠标移出控件时不取消hover
	bool  menu : 1;               // 菜单式鼠标管理器，1. 鼠标弹起时，如果不在pressitem上，则不触发click
  bool  destroying : 1;         // 正在被销毁(FinalRelease)
  bool  float_group_head : 1;   // 需要显示浮动组头
  bool  dragwnd_if_clickblank : 1; // 鼠标点击空白区域时，拖拽窗口
  bool  changeselection_onlbuttonup : 1; // 鼠标弹起时才修改选区
	
}LISTCTRLSTYLE;


// 消息映射ID分类 
// #define LISTCTRLBASE_KEYBOARDMOUSEMGR_PREMSG_ID   1   // mousemgr处理前先发给listctrl处理的消息映射ID
// #define LISTCTRLBASE_KEYBOARDMOUSEMGR_POSTMSG_ID  2   // mousemgr处理后再发给listctrl处理的消息映射ID
// #define LISTCTRLBASE_INNER_CONTROL_MSG_ID  3  // 内部控件ID。当内部控件调用DoNotify是，将发到这个ID上	
#define LISTCTRLBASE_EDIT_CONTROL_MSG_ID   4  // 编辑控件消息通知ID

struct UIAPI IListCtrlBase : public IControl
{
    UI_DECLARE_INTERFACE(ListCtrlBase);

    int   GetItemCount();
    int   GetVisibleItemCount();
    void  CalcFirstLastVisibleItem();
    void  SetCalcFirstLastVisibleItemFlag();

    bool  AddItem(IListItemBase* pItem);
    bool  InsertItem(IListItemBase* pItem, int nPos);
    bool  InsertItem(IListItemBase* pItem, IListItemBase* pInsertAfter);
    bool  InsertItem(IListItemBase* pItem, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST);
	void  RemoveItem(IListItemBase* pItem);
	void  RemoveItemFromTree(IListItemBase* pItem);
    void  RemoveItem(int nIndex);
    void  RemoveAllChildItems(IListItemBase* pParent);
    void  RemoveAllItem();
    void  DelayRemoveItem(IListItemBase* pItem);
	bool  MoveItem(IListItemBase*  p, IListItemBase* pNewParent, IListItemBase* pInsertAfter);

	void  ModifyListCtrlStyle(LISTCTRLSTYLE* add, LISTCTRLSTYLE* remove);
	bool  TestListCtrlStyle(LISTCTRLSTYLE* test);

    void  ToggleItemExpand(IListItemBase* pItem, bool bUpdate);
    void  CollapseItem(IListItemBase* pItem, bool bUpdate);
    void  ExpandItem(IListItemBase* pItem, bool bUpdate);
    void  CollapseAll(bool bUpdate);
    void  ExpandAll(bool bUpdate);

    IListItemBase*  GetHoverItem();
    IListItemBase*  GetPressItem();
    IListItemBase*  GetFocusItem();
	IListItemBase*  GetEditingItem();
	void  SetEditingItem(IListItemBase* pItem);
    void  SetHoverItem(IListItemBase* pItem);
    void  SetPressItem(IListItemBase* pItem);

    IObject*  GetHoverObject();
    IObject*  GetPressObject();
    void      SetFocusObject(IObject* pObj);
    IObject*  GetFocusObject();

    IListItemBase*  GetItemByPos(int i, bool bVisibleOnly=true);
    int  GetItemPos(IListItemBase*, bool bVisibleOnly = true);
    IListItemBase*  GetItemUnderCursor();
    IListItemBase*  GetItemById(long lId);
    IListItemBase*  FindItemByText(const wchar_t* szText, IListItemBase* pStart=nullptr);
    IListItemBase*  FindChildItemByText(const wchar_t* szText, IListItemBase* pParent=nullptr, IListItemBase* pStart=nullptr);
    IListItemBase*  GetItemByWindowPoint(POINT ptWindow);
    IListItemBase*  GetFirstItem();
    IListItemBase*  GetLastItem();
    IListItemBase*  GetFirstDrawItem();
    IListItemBase*  GetLastDrawItem();
    IListItemBase*  FindVisibleItemFrom(IListItemBase* pFindFrom=nullptr);
    IListItemBase*  EnumItemByProc(ListItemEnumProc pProc, IListItemBase* pEnumFrom = nullptr, WPARAM w = 0, LPARAM l = 0);
    IListItemBase*  GetFirstSelectItem();
    IListItemBase*  GetLastSelectItem();

	ICustomListItem*  InsertCustomItem(
		const wchar_t*  szLayoutName,
		long lId,
		IListItemBase* pParent = UITVI_ROOT, 
		IListItemBase* pInsertAfter = UITVI_LAST);
	ICustomListItem*  GetCustomListItem(IObject* p);


    IListItemShareData*  GetItemTypeShareData(int lType);
    void  SetItemTypeShareData(int lType, IListItemShareData* pData);
    void  RemoveItemTypeShareData(int lType);

    IScrollBarManager*  GetIScrollBarMgr();
    void  SetSortCompareProc(ListItemCompareProc p);
    void  Sort();
    void  SwapItemPos(IListItemBase*  p1, IListItemBase* p2);
    void  ItemRect2WindowRect(LPCRECT prc, LPRECT prcRet);
	void  WindowPoint2ItemPoint(IListItemBase* pItem, const POINT* pt, POINT* ptRet);
    void  ListItemDragDropEvent(UI::DROPTARGETEVENT_TYPE eEvent, IListItemBase* pItem);
    void  ListCtrlDragScroll();
    bool  IsItemRectVisibleInScreen(LPCRECT prc);

    IRenderBase*  GetFocusRender();
    void  SetFocusRender(IRenderBase* p);
    void  SetFocusItem(IListItemBase* pItem);
    void  SelectItem(IListItemBase* pItem, bool bNotify=true);
	void  ClearSelectItem(bool bNotify);
   
    void  InvalidateItem(IListItemBase* pItem);
    void  RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, IListItemBase* pItem);
    void  MakeItemVisible(IListItemBase* pItem, bool* pbNeedUpdate);

    void  UpdateListIfNeed();
    void  SetNeedLayoutItems();
    void  LayoutItem(IListItemBase* pStart, bool bRedraw);
    void  SetLayout(IListCtrlLayout* pLayout);
    IListCtrlLayout*  GetLayout();
    void  SetLayoutFixedHeight();
    void  SetLayoutVariableHeight();
	IListCtrlItemFixHeightFlowLayout*  SetLayoutFixedHeightFlow();
    void  UpdateItemIndex(IListItemBase* pStart);
	void  EnableInnerDrag(bool b);
	void  EnableReusable(IListCtrlReusableDelegate* p);

    short  GetVertSpacing();
    short  GetHorzSpacing();
    SIZE  GetAdaptWidthHeight(int nWidth, int nHeight);
    int   GetChildNodeIndent();
    void  SetChildNodeIndent(int n);
	void  GetItemContentPadding(REGION4*);
	void  SetItemContentPadding(REGION4*);

    void  SetMinWidth(int n);
    void  SetMinHeight(int n);
    int   GetMinWidth(); 
    int   GetMinHeight();
    int   GetMaxWidth(); 
    int   GetMaxHeight();

    int   GetItemHeight();
    void  SetItemHeight(int nHeight, bool bUpdate=true);

    void  Scroll2Y(int nY, bool bUpdate);
    void  Scroll2X(int nX, bool bUpdate);
    void  ScrollY(int nY, bool bUpdate);
    void  ScrollX(int nX, bool bUpdate);
    void  SetScrollPos(int nX, int nY, bool bUpdate);


	signal<IListCtrlBase*>&  SelectChangedEvent();
	signal<ListCtrlClickEventParam*>&  ClickEvent();

#if _MSC_VER >= 1800
    signal<IListCtrlBase*, IListItemBase*>&  RClickEvent();
    signal<IListCtrlBase*, IListItemBase*>&  MClickEvent();
    signal<IListCtrlBase*, IListItemBase*>&  DBClickEvent();
    signal<IListCtrlBase*, unsigned int, bool&>&  KeyDownEvent();
#else
	signal2<IListCtrlBase*, IListItemBase*>&  RClickEvent();
	signal2<IListCtrlBase*, IListItemBase*>&  MClickEvent();
	signal2<IListCtrlBase*, IListItemBase*>&  DBClickEvent();
	signal3<IListCtrlBase*, unsigned int, bool&>&  KeyDownEvent();
#endif
//     static void  SetListItemRectChangedCallback(
//                     bool (__stdcall *func)(IListItemBase& item,LPCRECT prcOld,LPCRECT prcNew));
};


class ListItemShareData;
interface UIAPI IListItemShareData : public IMessage
{
    IListCtrlBase*  GetListCtrlBase();

    UI_DECLARE_INTERFACE(ListItemShareData);
};


#define UI_MSGMAPID_LC_LAYOUT 142901215

// 向列表控件获取行间距
#define UI_LCM_LAYOUT_GET_LINESPACE  142901216
// 向列表控件获取item间距
#define UI_LCM_LAYOUT_GET_ITEMSPACE  142901217

// 该列表项单独占用一行
#define UI_LISTCTRL_LAYOUT_HORZ_SINGLELINE -3
// 该列表项占满当前行的剩余空间
//#define UI_LISTCTRL_LAYOUT_HORZ_REMAINLINE -4

// 每种布局要实现的基类
interface IListCtrlLayout
{
    virtual ~IListCtrlLayout() = 0 {};
    virtual void  SetIListCtrlBase(IListCtrlBase* p) = 0;
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent) = 0;
    virtual void  Measure(SIZE* pSize) = 0;
    virtual void  Release() = 0;
};

interface IListCtrlItemFixHeightFlowLayout : public IListCtrlLayout
{
	virtual void  SetLineSpace(int) = 0;
	virtual void  SetItemSpace(int) = 0;
};

interface IListCtrlReusableDelegate
{
	virtual void PreDrawItem(int row, int data) = 0;
	virtual void PostDrawItem(int row, int data) = 0;

	virtual int RowCount() = 0;
	virtual void ViewForRow() = 0;
};

}

#endif // _UI_LISTCTRLBASE_H_