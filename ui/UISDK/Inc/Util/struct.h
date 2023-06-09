#ifndef _UI_STRUCT_H_
#define _UI_STRUCT_H_

interface IDataObject;

namespace UI
{
//
// 9宫拉伸区域定义
//
class  UIAPI C9Region
{
public:
    C9Region(){ Set(0); }
    C9Region(const C9Region& o)
    {
		this->Copy(o);
    }
	bool IsEqual( const C9Region& r );
	void Copy( const C9Region& r );

	void Set( short w );
	void Set( short wLeftRight, short wTopBottom );
	void Set( short wLeft, short wTop, short wRight, short wBottom );
    bool IsAll_0();
    bool IsAll_1();
	bool IsSimpleValue();
	bool IsSimpleRect();
	void DpiScale();
	void DpiRestore();
	
    
	short topleft;
	short top;
	short topright;
	short left;
	short right;
	short bottomleft;
	short bottom;
	short bottomright;
};

interface IRenderTarget;
struct RENDERBASE_DRAWSTATE
{
    IRenderTarget*  pRenderTarget;
    RECT  rc;
    int  nState;
};

struct TEXTRENDERBASE_DRAWSTATE
{
    RENDERBASE_DRAWSTATE  ds_renderbase;
    LPCTSTR  szText;
    int  nDrawTextFlag;
};

enum
{
    TOOLTIP_ACTION_FLAG_NORMAL = 0,              // 正常系统提示条，悬浮控件时，计时显示；移开控件时，立即隐藏
};

interface IMessage;
// ShowToolTip的参数
struct TOOLTIPITEM
{
    // 要提示的内容（如果该值有效，则忽略pNotifyObj/pItemData）
    LPCTSTR  szToolTipContent;

    // 要显示提示条的控件，以及消息通知的对象
    IMessage*  pNotifyObj;   

    // 如果是要显示控件内部一个item的提示信息，可将该值设置为合适的值，
    // 然后mgr会向pNotifyObj发送UI_WM_GET_TOOLTIPINFO消息
    void*  pItemData;    

    // 提示条的行为标志。 （20120928 现在只支持normal）
    int   nToolTipFlag; 
};


class TimerItem;
typedef void  (*UITimerProc)(UINT_PTR nTimerID, TimerItem* pTimerItem);

interface IMessage;
class TimerItem
{
public:
    int  nRepeatCount;   // 响应次数,-1表示无限

    IMessage* pNotify;    // 响应WM_TIMER的对象
    UITimerProc  pProc;
	std::function<bool(UINT_PTR, TimerItem*)>  func;

	UINT_PTR  nId;
    WPARAM   wParam;
    LPARAM   lParam; 

    TimerItem()
    {
        nRepeatCount = -1;
        pNotify = nullptr;
        pProc = nullptr;
        wParam = 0;
        lParam = 0;
        nId = 0;
    }
};


// UI_WM_DROPTARGETEVENT 消息参数
enum DROPTARGETEVENT_TYPE
{
	_DragEnter,
	_DragOver,
	_DragLeave,
	_Drop,
};
interface IDragFeedback;
struct DROPTARGETEVENT_DATA
{
	IDataObject*  pDataObj;
	DWORD         grfKeyState;
	POINTL        pt;
	DWORD*        pdwEffect;
	IDragFeedback*  pDragFeedback;
};

}
#endif // _UI_STRUCT_H_