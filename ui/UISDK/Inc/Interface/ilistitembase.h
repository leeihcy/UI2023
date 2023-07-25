#ifndef _UI_ILISTITEMBASE_H_
#define _UI_ILISTITEMBASE_H_
#include "imessage.h"

namespace ui
{
interface IPanel;
class ListItemBase;
interface IListCtrlBase;
class RenderContext;

//////////////////////////////////////////////////////////////////////////
// ListItem派生类消息

// 在绘制之前处理延迟操作
#define UI_LISTITEM_MSG_DELAY_OP  139222216

// 获取item渲染状态，用于绘制自己
#define UI_LISTITEM_MSG_GETRENDERSTATE 147201333

// 列表控件向列表项发送拖拽释放时的状态
// wParam: 状态DROPTARGETEVENT_TYPE
#define  UI_LISTITEM_MSG_DRAGDROP_EVENT  141431104

enum LISTITEM_DELAY_OP
{
    // 在控件的大小发生改变后，通知子对象。如果子对象目前没有处于可视区域，则稍后在自己可见时再更新自己的布局
    DELAY_OP_SIZE_CHANGED = 0x01,

    // 在控件布局重新改变之后，通知子对象。子对象在自己刷新的时候再更新自己的布局
    DELAY_OP_LAYOUT_CHANGED = 0x02,

    // 首次绘制，例如加载头像、数据、背景等
    DELAY_OP_LAZYLOADDATA = 0x04,
};

// lbuttondown_return_value
enum ITEMMOUSEKEYEVENTRET
{
    IMKER_NONE = 0,    
    IMKER_HANDLED_BY_ITEM = 0x0001,         //  已处理
    IMKER_HANDLED_BY_INNERCONTROL = 0x0002, //  已处理
    IMKER_DONT_CHANGE_SELECTION = 0x0004,   //  不要改变当前的选择项
};

// Style
typedef struct tagLISTITEMSTYLE
{
    bool  bNotSelectable:1;  // 不能被选中。例如分隔条
    bool  bNotFocusable:1;   // 不能有焦点。
    bool  bNoChildIndent:1;  // 该结点不计算缩进
    bool  bFloat:1;          // 该ITEM正在浮动（不受滚动偏移量影响），如浮动组头
    bool  bOwnerDraw:1;      // 是否将item的绘制消息先发送ListCtrl; ControlStyle中的ownerdraw表示发送给外部窗口
	bool  bDelayRemoving:1;  // 正在延时删除中，不要再调用其它相关操作
    bool  bMoveAnimating:1;  // 正在做动画。用于CalcFirstLastDrawItem检查
	bool  bLayer : 1;        // 启用分层，以显示硬件加速动画

}LISTITEMSTYLE, *LPLISTITEMSTYLE;

interface __declspec(uuid("08D8FC93-CA95-42D4-B804-811F80D9F080"))
UIAPI IListItemBase : public IMessage
{
    void  SetIListCtrlBase(IListCtrlBase* p);
    IListCtrlBase*  GetIListCtrlBase();

    IListItemBase*  GetNextItem();
    IListItemBase*  GetPrevItem();
    IListItemBase*  GetParentItem();
    IListItemBase*  GetChildItem();
    IListItemBase*  GetLastChildItem();
    IListItemBase*  GetPrevSelection();
    IListItemBase*  GetNextSelection();

    IListItemBase*  GetNextVisibleItem();
    IListItemBase*  GetPrevVisibleItem();
    IListItemBase*  GetNextTreeItem(); 
    IListItemBase*  GetPrevTreeItem();

    bool  IsMyChildItem(IListItemBase* pChild, bool bTestdescendant);
    bool  HasChild();

    void  SetNextItem(IListItemBase* p);
    void  SetPrevItem(IListItemBase* p);
    void  SetParentItem(IListItemBase* p);
    void  SetChildItem(IListItemBase* p);
    void  SetLastChildItem(IListItemBase* p);
    void  SetNextSelection(IListItemBase* p);
    void  SetPrevSelection(IListItemBase* p);
	void  AddChild(IListItemBase* p);
	void  AddChildFront(IListItemBase* p);

    void  GetParentRect(LPRECT prc);
    void  SetParentRect(LPCRECT prc);
    void  SetParentRectLightly(LPCRECT prc);
    void  SetFloatRect(LPCRECT prc);

	LONG_PTR  GetId();
    void  SetId(LONG_PTR n);
    int   GetItemType();
    void  SetItemType(long);
    unsigned int  GetItemFlag();
    void  SetItemFlag(int n);
    void  SetText(const TCHAR* szText);
    const TCHAR*  GetText();
    void  SetToolTip(const TCHAR* szText);
    const TCHAR*  GetToolTip();
	long*  GetData();
    void  SetData(long* p);
	long*  GetCtrlData();
    void  SetCtrlData(long* p);
    int   GetLineIndex() ;
    void  SetLineIndex(int n);
    void  SetTreeIndex(int n);
    void  SetNeighbourIndex(int n);
    int   GetTreeIndex();
    int   GetNeighbourIndex();
    int   CalcDepth();
    void  SetConfigWidth(int n);
    void  SetConfigHeight(int n);
    int   GetConfigWidth();
    int   GetConfigHeight();
    long  GetSort();
    void  SetSort(long);
	void  SetFloat(bool);

    void  GetDesiredSize(SIZE* pSize);

    IRenderBase*  GetIconRender();
    void  SetIconRender(IRenderBase* p);
    void  SetIconRenderType(RENDER_TYPE eType);
    void  SetIconFromFile(const TCHAR* szIconPath);
    void  SetIconFromImageId(const TCHAR* szImageId);

    unsigned int  GetItemState();
    unsigned int  GetItemDelayOp();
    void  AddItemDelayOp(int n);
    void  RemoveDelayOp(int n);
    void  ClearDelayOp();
    bool  IsMySelfVisible();
    bool  IsVisible();
    bool  IsDisable();
    bool  IsHover();
    bool  IsPress();
    bool  IsFocus();
    bool  IsCollapsed();
    bool  IsExpand();
    bool  IsSelected();
    bool  IsSelectable();
    bool  IsChecked();
    bool  IsRadioChecked();
    bool  IsDragDropHover();
    bool  IsFloat();

    void  ModifyStyle(LISTITEMSTYLE* pAdd, LISTITEMSTYLE* pRemove);
    bool  TestStyle(const LISTITEMSTYLE& s);

    void  SetPress(bool b, bool bNotify=true);
    void  SetHover(bool b, bool bNotify=true);
    void  SetExpand(bool b, bool bNotify=true);
    void  SetFocus(bool b, bool bNotify=true);
    void  SetSelected(bool b, bool bNotify=true);
    void  SetDisable(bool b, bool bNotify=true);
    void  SetChecked(bool b, bool bNotify=true);
    void  SetRadioChecked(bool b, bool bNotify=true);
    void  SetDragDropHover(bool b, bool bNotify=true);
    void  SetSelectable(bool b);
	void  SetVisible(bool b);


    IPanel*  GetRootPanel();
    bool  CreateRootPanel();
    void  DrawItemInnerControl(IRenderTarget* pRenderTarget);
    int   GetItemRenderState();
    void  Invalidate();

	// 动画
	void  SetOpacity(const unsigned char alpha, LayerAnimateParam* param = nullptr);
	ILayer*  GetLayerForAnimate();

	UI_DECLARE_INTERFACE(ListItemBase);
};

class CustomListItem;
interface UIAPI ICustomListItem : public IListItemBase
{
	enum {FLAG = 162712129};
	IObject*  FindControl(const wchar_t* szId);

	void  SetStringData(LPCSTR data);
	LPCSTR  GetStringData();

    UI_DECLARE_INTERFACE(CustomListItem);
};

}

#endif // _UI_ILISTITEMBASE_H_