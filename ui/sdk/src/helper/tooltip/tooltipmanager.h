#pragma once
#include "src/util/Timeout/timeout.h"
#include "include/interface/iuiinterface.h"

namespace ui
{
#define TOOLTIP_DELAY_SHOW_TIME    600
#define TOOLTIP_AUTO_HIDE_TIME    (GetDoubleClickTime()*10)
#define TOOLTIP_MAX_WIDTH  500/*256*/
	class Application;

//////////////////////////////////////////////////////////////////////////
// TimerProc thunk

class ToolTipItem
{
public:
    ToolTipItem();

public:
    void  Set(TOOLTIPITEM* pItem);
    void  Reset();
    
public:
    std::string  strContent;

    IMessage*  pNotifyObj;   
    void*  pItemData;    

    int  nToolTipFlag; 
};

//
// 提示条的管理
// 用于管理各种计时器及消息转发，并实现各种逻辑，使得IToolTipUI和控件不用去管这些逻辑
//
class ToolTipManager : public ITimeoutCallback
{
public:
	ToolTipManager();
	~ToolTipManager();

	void   Init(Application* p, IToolTipUI* pTooltipUI=nullptr);
	void   Release();
	bool   Hide();
	bool   Show(TOOLTIPITEM* pItemInfo);

protected:
    virtual void OnTimeout(long lId, long wParam, long lParam) override;

protected:
	IToolTipUI*        m_pToolTipUI;      // 就是控件的tooltip
	ToolTipItem        m_tooltipItem;     // 当前正在显示或者正在等待显示的提示条信息
    Application*     m_pUIApplication;

	enum TIMEOUT_TYPE
	{
		TIMEOUT_DELAY_SHOW,  // 延时显示
		TIMEOUT_AUTO_HIDE,   // 提示条显示时间，然后自动隐藏
	};
    CTimeout   m_timeout;
};

}

