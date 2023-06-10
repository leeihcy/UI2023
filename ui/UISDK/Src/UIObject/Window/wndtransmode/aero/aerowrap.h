#pragma once
#include "../wndtransmodebase.h"
#include "Src\Util\dwm\dwmhelper.h"

//
// 2014.5.22 16:12 
//
// 废弃在没有WS_CAPTION样式的情况下，使用TRANSPARENT模式的aero透明，主要存在以下几个问题无法解决:
// 1. 最大化时，透明效果丢失，背景变成全黑
// 2. 快速拖动窗口大小时，还是有机率出现黑色背景，造成闪烁
// 3. 切换系统样式，有时会导致窗口出现很浓的阴影无法消失。
//
// (这两个问题，在有WS_CAPTION样式时不存在; 
//  另外在没有WS_CAPTION，有WM_THICKFRAME，但不处理WM_NCCALCSIZE消息的情况下也是好的，
//  所以无边框的窗口还是用blur模式去处理吧)
//  

namespace UI
{

class AeroWindowWrap: public WndTransModeBase
{
public:
    AeroWindowWrap();
    ~AeroWindowWrap();

    VIRTUAL_BEGIN_MSG_MAP(LayeredWindowWrap)
#if(_WIN32_WINNT >= 0x0600)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, _OnDwmCompositionChanged)
#endif
        CHAIN_MSG_MAP_MEMBER_P(m_pAeroDisableMode);
    END_MSG_MAP()

	UI_BEGIN_MSG_MAP()
        UICHAIN_MSG_MAP_POINT_MEMBER(m_pAeroDisableMode)
        UIMSG_QUERYINTERFACE(AeroWindowWrap)
	UI_END_MSG_MAP()


    LRESULT  _OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT  _OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    virtual WINDOW_TRANSPARENT_TYPE  GetType();
    virtual bool  RequireAlphaChannel();
    virtual void  UpdateRgn();
    virtual void  Enable(bool b);
    virtual bool  Commit();
    virtual void  Init(ICustomWindow* pWnd);

    IAeroWindowWrap*  GetIAeroWndTransMode();
    AERO_MODE  GetAeroMode();
    void  GetBlurRegion(CRegion4* pregion);
	void  SetTransparentMargins(RECT* prcMargin);

public:

    AERO_MODE  m_eMode;
    union
    {
        struct TransparentMode
        {
            MARGINS  m_margins;
        }trans;

        struct BlurMode
        {
            RECT  m_regionBlur;  // 指定四周不进行blur的区域
            HRGN  m_hrgnBlurRgn;  // <-- 外部设置
            bool  m_bAutoClipChildCtrl;  // 自动剪裁掉子控件
        }blur;
    };    

    DwmHelper*  m_pDWM;
    WINDOW_TRANSPARENT_TYPE  m_eAeroDisableMode;
    IWindowTransparent*  m_pAeroDisableMode;

    IAeroWindowWrap*  m_pIAeroWindowWrap;
};


}