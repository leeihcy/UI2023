#pragma once
#include "Inc\Interface\icustomwindow.h"
#include "Src\UIObject\Window\window.h"
#include "Inc\Interface\iwndtransmode.h"

/*
 非客户区重绘的一些东西：

. WM_NCACTIVATE:
  如果拦截了这个消息，aero transparent将在激活时不会将阴影修改为厚阴影
  如果放开这个消息，在经典主题下面，窗口失焦会刷出标题栏

. WM_NCCALCSIZE
  用于修改客户区位置。直接return TRUE;即可屏蔽掉非客户区

. WM_NCHITTEST
  鼠标在WM_NCHITTEST如果返回HTCLOSE,HTMAXBUTTON,HTMINBUTTON，系统会强制刷出这三个按钮，需要屏蔽掉

. WM_NCPAINT
  如果屏蔽该消息，那aero trans不会再显示阴影了。
  如果放开这个消息，在经典主题下面窗口得到激活时将刷出标题栏（目前只有anti模式下发现有该问题，mask color/mask alpha还有待验证）

. 经典模式下面的WM_SETTEXT,WM_SETICON (WM_NCLBUTTONDOWN??)
  调用该api，系统会强制刷一下标题样，如何解决?
  http://blog.sina.com.cn/s/blog_437fd95e01013lrk.html
  Chrome: ui\views\widget\native_widget_win.cc  void NativeWidgetWin::LockUpdates();
  将窗口WS_VISIBLE属性去掉，然后再加回来

  另外还有：WM_NCLBUTTONDOWN、EnableMenuItem(called from our WM_INITMENU handler)

. 两个没记载的消息： （目前测试还没发现它们的神奇之处）
  WM_NCUAHDRAWCAPTION
  WM_NCUAHDRAWFRAME

*/
// These two messages aren't defined in winuser.h, but they are sent to windows  
// with captions. They appear to paint the window caption and frame.  
// Unfortunately if you override the standard non-client rendering as we do  
// with CustomFrameWindow, sometimes Windows (not deterministically  
// reproducibly but definitely frequently) will send these messages to the  
// window and paint the standard caption/title over the top of the custom one.  
// So we need to handle these messages in CustomFrameWindow to prevent this  
// from happening.  
#define WM_NCUAHDRAWCAPTION 0xAE
#define WM_NCUAHDRAWFRAME   0xAF  


namespace UI
{

class LayeredWindowWrap;
class AeroWindowWrap;

//
// 自定义窗口类，无非客户区域
//
class CustomWindow : public Window
{
public:
	CustomWindow(ICustomWindow*);
	~CustomWindow(void);

	VIRTUAL_BEGIN_MSG_MAP(CustomWindow)
        MESSAGE_HANDLER(WM_NCCALCSIZE, _OnNcCalcSize)
        CHAIN_MSG_MAP_MEMBER_P(m_pWindowTransparent)
//		MESSAGE_HANDLER(WM_NCPAINT, _OnNcPaint)
        MESSAGE_HANDLER(WM_NCHITTEST, _OnNcHitTest)
		MESSAGE_HANDLER(WM_NCACTIVATE, _OnNcActivate)
        MESSAGE_HANDLER(WM_SETTEXT, _OnSetTextIcon)
        MESSAGE_HANDLER(WM_SETICON, _OnSetTextIcon)
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, _OnWindowPosChanging)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, _OnGetMinMaxInfo)
        MESSAGE_HANDLER(WM_NCUAHDRAWCAPTION, _OnNCUAHDrawCaption)
        MESSAGE_HANDLER(WM_NCUAHDRAWFRAME, _OnNCUAHDrawFrame)
#if(_WIN32_WINNT >= 0x0600)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, _OnDwmCompositionChanged)
#endif
		CHAIN_MSG_MAP(Window)
	END_MSG_MAP()

	UI_BEGIN_MSG_MAP()
        UICHAIN_MSG_MAP_POINT_MEMBER(m_pWindowTransparent)
		UIMSG_ERASEBKGND(OnEraseBkgnd)
		UIMSG_HITTEST(OnHitTest)
        MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		
//		UIMSG_NCHITTEST(OnNcHitTest)
// 		UIMSG_NCMOUSEMOVE    (OnNcMouseMove)
// 		UIMSG_NCMOUSEHOVER   (OnNcMouseHover)
// 		UIMSG_NCMOUSELEAVE   (OnNcMouseLeave)
// 		UIMSG_NCLBUTTONDOWN  (OnNcLButtonDown)
// 		UIMSG_NCLBUTTONUP    (OnNcLButtonUp)
// 		UIMSG_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
// 		UIMSG_NCRBUTTONDOWN  (OnNcRButtonDown)
// 		UIMSG_NCRBUTTONUP    (OnNcRButtonUp)
// 		UIMSG_NCRBUTTONDBLCLK(OnNcRButtonDblClk)
// 		UIMSG_NCMBUTTONDOWN  (OnNcMButtonDown)
// 		UIMSG_NCMBUTTONUP    (OnNcMButtonUp)
// 		UIMSG_NCMBUTTONDBLCLK(OnNcMButtonDblClk)

        UIMSG_GET_WINDOW_TRANSPARENT_MODE(GetWindowTransparentType)
        UIMSG_QUERYINTERFACE(CustomWindow)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_PRECREATEWINDOW(PreCreateWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(Window)

    ICustomWindow* GetICustomWindow() { return m_pICustomWindow; }

	// 公共方法
public:
	void  SetResizeCapability(long nType);
    long  GetResizeCapability();
	void  UpdateWindowRgn();
	
    bool  IsWindowLayered();
    void  EnableWindowLayered(bool b);
    void  EnableWindowAero(bool b);
    void  SetWindowTransparentType(WINDOW_TRANSPARENT_TYPE l);
    void  SetWindowTransparent(IWindowTransparent* pMode);
    WINDOW_TRANSPARENT_TYPE  GetWindowTransparentType();
    IWindowTransparent*  GetWindowTransparentPtr();

protected:
	void  OnSerialize(SERIALIZEDATA* pData);

private:
	BOOL  PreCreateWindow(CREATESTRUCT* pcs);

	virtual void  virtualInnerInitWindow() override;
	virtual bool  virtualCommitReq() override;
	virtual void  virtualOnPostDrawObjectErasebkgnd() override;

    bool  IsBorderlessWindow();

	// 消息响应
protected:
    
    LRESULT  _OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnGetMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
//	LRESULT  _OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnSetTextIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnNCUAHDrawCaption(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnNCUAHDrawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    
	void  OnEraseBkgnd(IRenderTarget* hDC);
    int   OnHitTest(POINT* pt, POINT*  ptInChild);
    void  OnSysCommand(UINT nID, CPoint point);
	void  OnLButtonDblClk(UINT nFlags, POINT point);


    virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);

//	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
//	LRESULT  OnNcHitTest( POINT pt );
// 	void OnNcMouseMove( UINT nHitTest, POINT point );
// 	void OnNcMouseHover( UINT nHitTest, POINT point ); 
// 	void OnNcMouseLeave();
// 	void OnNcLButtonDown( UINT nHitTest, POINT point );
// 	void OnNcLButtonUp( UINT nHitTest, POINT point );
// 	void OnNcLButtonDblClk( UINT nHitTest, POINT point );
// 	void OnNcRButtonDown( UINT nHitTest, POINT point );
// 	void OnNcRButtonUp( UINT nHitTest, POINT point );
// 	void OnNcRButtonDblClk( UINT nHitTest, POINT point );
// 	void OnNcMButtonDown( UINT nHitTest, POINT point );
// 	void OnNcMButtonUp( UINT nHitTest, POINT point );
// 	void OnNcMButtonDblClk( UINT nHitTest, POINT point );

protected:
	bool    TestResizeBit(int nBit);

protected:
    ICustomWindow*  m_pICustomWindow;
    
    //
	// 窗口透明处理相关数据
	//
    IWindowTransparent*  m_pWindowTransparent;
	bool  m_bNeedToSetWindowRgn;   // 是否需要重新设置窗口异形区域
    bool  m_bFirstNcCalcSize;
    bool  m_bRemoveMAXBOX;        // 最大化时去掉MAXBOX样式
    bool  m_bAeroCaptionAnimate;  // 使用aero ws_caption 动画（显示、隐藏、最大小、最小化）

    //
    // 边框拖拽
    //
	long  m_lResizeCapability;  // 窗口边缘伸缩能力标志位
	long  m_lResizeBorder;      // 用于拖拽的边缘区域大小
    long  m_lMaximizeBorder;    // 最大化时需要位于屏幕外的边框大小，可以与resizeborder不一样

    friend class AeroWindowWrap;
    friend class LayeredWindowWrap;
};

}
