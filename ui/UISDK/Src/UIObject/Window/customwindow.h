#pragma once
#include "include/interface/icustomwindow.h"
#include "Src\UIObject\Window\window.h"
#include "include/interface/iwndtransmode.h"

/*
 �ǿͻ����ػ��һЩ������

. WM_NCACTIVATE:
  ��������������Ϣ��aero transparent���ڼ���ʱ���Ὣ��Ӱ�޸�Ϊ����Ӱ
  ����ſ������Ϣ���ھ����������棬����ʧ����ˢ��������

. WM_NCCALCSIZE
  �����޸Ŀͻ���λ�á�ֱ��return TRUE;�������ε��ǿͻ���

. WM_NCHITTEST
  �����WM_NCHITTEST�������HTCLOSE,HTMAXBUTTON,HTMINBUTTON��ϵͳ��ǿ��ˢ����������ť����Ҫ���ε�

. WM_NCPAINT
  ������θ���Ϣ����aero trans��������ʾ��Ӱ�ˡ�
  ����ſ������Ϣ���ھ����������洰�ڵõ�����ʱ��ˢ����������Ŀǰֻ��antiģʽ�·����и����⣬mask color/mask alpha���д���֤��

. ����ģʽ�����WM_SETTEXT,WM_SETICON (WM_NCLBUTTONDOWN??)
  ���ø�api��ϵͳ��ǿ��ˢһ�±���������ν��?
  http://blog.sina.com.cn/s/blog_437fd95e01013lrk.html
  Chrome: ui\views\widget\native_widget_win.cc  void NativeWidgetWin::LockUpdates();
  ������WS_VISIBLE����ȥ����Ȼ���ټӻ���

  ���⻹�У�WM_NCLBUTTONDOWN��EnableMenuItem(called from our WM_INITMENU handler)

. ����û���ص���Ϣ�� ��Ŀǰ���Ի�û�������ǵ�����֮����
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


namespace ui
{

class LayeredWindowWrap;
class AeroWindowWrap;

//
// �Զ��崰���࣬�޷ǿͻ�����
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

	// ��������
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

	// ��Ϣ��Ӧ
protected:
    
    long  _OnNcCalcSize(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnWindowPosChanging(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnNcHitTest(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnGetMinMaxInfo( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
//	long  _OnNcPaint(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	long  _OnNcActivate(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnSetTextIcon(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnNCUAHDrawCaption(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnNCUAHDrawFrame(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnDwmCompositionChanged(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    
	void  OnEraseBkgnd(IRenderTarget* hDC);
    int   OnHitTest(POINT* pt, POINT*  ptInChild);
    void  OnSysCommand(unsigned int nID, CPoint point);
	void  OnLButtonDblClk(unsigned int nFlags, POINT point);


    virtual void  virtualOnSize(unsigned int nType, unsigned int nWidth, unsigned int nHeight);

//	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
//	long  OnNcHitTest( POINT pt );
// 	void OnNcMouseMove( unsigned int nHitTest, POINT point );
// 	void OnNcMouseHover( unsigned int nHitTest, POINT point ); 
// 	void OnNcMouseLeave();
// 	void OnNcLButtonDown( unsigned int nHitTest, POINT point );
// 	void OnNcLButtonUp( unsigned int nHitTest, POINT point );
// 	void OnNcLButtonDblClk( unsigned int nHitTest, POINT point );
// 	void OnNcRButtonDown( unsigned int nHitTest, POINT point );
// 	void OnNcRButtonUp( unsigned int nHitTest, POINT point );
// 	void OnNcRButtonDblClk( unsigned int nHitTest, POINT point );
// 	void OnNcMButtonDown( unsigned int nHitTest, POINT point );
// 	void OnNcMButtonUp( unsigned int nHitTest, POINT point );
// 	void OnNcMButtonDblClk( unsigned int nHitTest, POINT point );

protected:
	bool    TestResizeBit(int nBit);

protected:
    ICustomWindow*  m_pICustomWindow;
    
    //
	// ����͸�������������
	//
    IWindowTransparent*  m_pWindowTransparent;
	bool  m_bNeedToSetWindowRgn;   // �Ƿ���Ҫ�������ô�����������
    bool  m_bFirstNcCalcSize;
    bool  m_bRemoveMAXBOX;        // ���ʱȥ��MAXBOX��ʽ
    bool  m_bAeroCaptionAnimate;  // ʹ��aero ws_caption ��������ʾ�����ء����С����С����

    //
    // �߿���ק
    //
	long  m_lResizeCapability;  // ���ڱ�Ե����������־λ
	long  m_lResizeBorder;      // ������ק�ı�Ե�����С
    long  m_lMaximizeBorder;    // ���ʱ��Ҫλ����Ļ��ı߿��С��������resizeborder��һ��

    friend class AeroWindowWrap;
    friend class LayeredWindowWrap;
};

}
