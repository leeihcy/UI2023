#pragma once
#include "..\wndtransmodebase.h"
namespace UI
{

//
//	分层窗口实现代码
//
class LayeredWindowWrap : public WndTransModeBase
{
public:
    LayeredWindowWrap();
    ~LayeredWindowWrap();

    VIRTUAL_BEGIN_MSG_MAP(LayeredWindowWrap)
		MESSAGE_HANDLER(WM_SIZE, _OnSize)
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, _OnWindowPosChanged)
        MESSAGE_HANDLER(WM_CANCELMODE, _OnCancelMode)
        MESSAGE_HANDLER(WM_NCHITTEST, _OnNcHitTest)
        MESSAGE_HANDLER(WM_SETCURSOR, _OnSetCursor)
    END_MSG_MAP()


	UI_BEGIN_MSG_MAP()
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_QUERYINTERFACE(LayeredWindowWrap)
	UI_END_MSG_MAP()

    LRESULT  _OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnMouseMove(UINT nFlags, POINT point);

    ILayeredWindowWrap*  GetILayeredWindowWrap();
    byte  GetAlpha();
    void  SetAlpha(byte b, bool bUpdate);

    virtual WINDOW_TRANSPARENT_TYPE  GetType()
    {
        return WINDOW_TRANSPARENT_TYPE_LAYERED;
    }
    virtual bool  RequireAlphaChannel()
    {
        return true;
    }
    virtual void  Init(ICustomWindow* pWnd)
    {
        __super::Init(pWnd);
        GetWindowPos();
    }
    virtual void  UpdateRgn(); 
    bool  UpdateLayeredCaptionWindowRgn();
    virtual bool  Commit();
    virtual void  Enable(bool b);

    void   GetWindowPos();
    void   OnWindowPosChanged(LPWINDOWPOS lpWndPos);

    void   OnLButtonDown(UINT nHitTest);
    void   OnEnterSizeMove(UINT nHitTest);
    void   OnExitSizeMove();

protected:
    bool   IsMinimized();

protected:
    friend    class CustomWindow;
    ILayeredWindowWrap*  m_pILayeredWindowWrap;

    // TODO: 当分层窗口带THICKFRAME样式时，最大化后窗口区域与客户区域不一致，
    //       那 m_ptWindow应该表示窗口左上角，还是客户区域左上角？
    //      (最大化时会导致窗口来回偏移好几次)

    POINT     m_ptWindow;          // 分层窗口的坐标
    SIZE      m_sizeWindow;        // 分层窗口的大小(客户区大小，不是窗口大小，与buffer的大小必须一致，否则渲染失败)

    // 分层窗口拉伸时使用的中间参数
    UINT      m_nHitTestFlag;      // 拉伸标识
    POINT     m_ptStartSizeMove;   // 开始拉伸时，鼠标的位置，用于计算鼠标偏移
    POINT     m_ptWindowOld;       // 开始拉伸时的窗口坐标，用于和偏移一起计算新位置
    SIZE      m_sizeWindowOld;     // 开始拉伸时的窗口大小，用于和偏移一起计算新大小
};

}