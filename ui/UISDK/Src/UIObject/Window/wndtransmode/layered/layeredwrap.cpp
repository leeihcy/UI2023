#include "stdafx.h"
#include "layeredwrap.h"
#include "Src/UIObject/Window/customwindow.h"
#include "Src/Layer/layer.h"
#include "Src/Layer/software_layer.h"

// WHERE_NOONE_CAN_SEE_ME
#define WINDOWS_MINIMIZED_POINT  -32000  // 窗口最小化后，window传递给我们的位置
namespace ui
{


LayeredWindowWrap::LayeredWindowWrap()
{
    m_nHitTestFlag = 0;
    m_ptStartSizeMove.x = 0;
    m_ptStartSizeMove.y = 0;
    m_ptWindowOld.x = 0;
    m_ptWindowOld.y = 0;
    m_sizeWindowOld.cx = 0;
    m_sizeWindowOld.cy = 0;
    m_ptWindow.x = NDEF;
    m_ptWindow.y = NDEF;
    m_sizeWindow.cx = NDEF;
    m_sizeWindow.cy = NDEF;
    m_pILayeredWindowWrap = nullptr;
}
LayeredWindowWrap::~LayeredWindowWrap()
{
    SAFE_DELETE(m_pILayeredWindowWrap);
}

void  LayeredWindowWrap::GetWindowPos()
{
	HWND hWnd = GetHWND();

    CRect rc;
    ::GetClientRect(hWnd, &rc);
    MapWindowPoints(hWnd, nullptr, (LPPOINT)&rc, 2);

    m_sizeWindow.cx = rc.Width();
    m_sizeWindow.cy = rc.Height();
    m_ptWindow.x = rc.left;
    m_ptWindow.y = rc.top;
}


// 模拟拖拽窗口拉伸过程
void  LayeredWindowWrap::OnLButtonDown(unsigned int nHitTest)
{
    OnEnterSizeMove(nHitTest);
}
void  LayeredWindowWrap::OnLButtonUp(unsigned int nFlags, POINT point)
{
    OnExitSizeMove();
}
void  LayeredWindowWrap::OnMouseMove(unsigned int nFlags, POINT point)
{
    if (0 == m_nHitTestFlag)
        return;

    POINT ptCursor;
    GetCursorPos(&ptCursor);

    int nxMoveDiff = 0;
    int nyMoveDiff = 0;

    int oldCX = m_sizeWindow.cx;
    int oldCY = m_sizeWindow.cy;

    // 计算窗口的新坐标 (注：对于向左/上拉伸时，如果限制了最大宽/高，则在计算坐标时需要按照最大宽/高来计算，而不是鼠标位置)
    switch(m_nHitTestFlag)
    {
    case HTLEFT:
        nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
            m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
        if (m_pWindow->m_lMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_lMinWidth)
            m_sizeWindow.cx = m_pWindow->m_lMinWidth;

        m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;

        break;

    case HTRIGHT:
        nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;
        break;

    case HTTOP:
        nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

        if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
            m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
        if (m_pWindow->m_lMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_lMinHeight)
            m_sizeWindow.cy = m_pWindow->m_lMinHeight;

        m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;

        break;

    case HTBOTTOM:
        nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
        break;

    case HTTOPLEFT:
        nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

        if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
            m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
        if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
            m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
        if (m_pWindow->m_lMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_lMinWidth)
            m_sizeWindow.cx = m_pWindow->m_lMinWidth;
        if (m_pWindow->m_lMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_lMinHeight)
            m_sizeWindow.cy = m_pWindow->m_lMinHeight;

        m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;
        m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;

        break;

    case HTTOPRIGHT:
        nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

        if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
            m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
        if (m_pWindow->m_lMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_lMinHeight)
            m_sizeWindow.cy = m_pWindow->m_lMinHeight;

        m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;
        break;

    case HTBOTTOMLEFT:
        nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
            m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
        if (m_pWindow->m_lMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_lMinWidth)
            m_sizeWindow.cx = m_pWindow->m_lMinWidth;

        m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;

        nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
        break;

    case HTBOTTOMRIGHT:
        nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
        break;
    }

    // 限制窗口大小
    if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
        m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
    if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
        m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
    if (m_pWindow->m_lMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_lMinWidth)
        m_sizeWindow.cx = m_pWindow->m_lMinWidth;
    if (m_pWindow->m_lMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_lMinHeight)
        m_sizeWindow.cy = m_pWindow->m_lMinHeight;

    if (oldCX == m_sizeWindow.cx && oldCY == m_sizeWindow.cy)
    {
        return;
    }


    //m_pWindow->SetCanRedraw(false);  // 缓存被清空，防止此期间有object redraw,提交到窗口时，数据不完整。
    //m_pWindow->CreateDoubleBuffer(m_sizeWindow.cx, m_sizeWindow.cy);
    m_pWindow->notify_WM_SIZE(0, m_sizeWindow.cx, m_sizeWindow.cy);

    // 注意：m_rcParent的更新千万不能使用GetWindowRect。因为窗口的大小现在就没有变
    //       所以这里也就没有采用SendMessage(WM_SIZE)的方法
    SetRect(&m_pWindow->m_rcParent, 0,0, m_sizeWindow.cx, m_sizeWindow.cy);

    m_pWindow->SetConfigWidth(m_sizeWindow.cx);
    m_pWindow->SetConfigHeight(m_sizeWindow.cy);

    m_pWindow->UpdateLayout();

    //m_pWindow->SetCanRedraw(true);
    m_pWindow->Invalidate();

    // 模拟窗口大小改变消息
    // PS: 20130108 17:07 将消息发送的时机移到OnSize之后。
    //     主要是因为在OnSize中，会触发richedit的setcaretpos通知，然后此时窗口的大小还没有真正改变，
    //     因此在CCaretWindow::SetCaretPos中调用MapWindowPoints获取到的光标屏幕位置还是旧的，导致光标
    //     跟随窗口移动失败。因此将CCaretWindow::OnSyncWindowPosChanging延后来修正这个问题。
    {
        MSG  msg;
        msg.hwnd = GetHWND();
        msg.message = WM_WINDOWPOSCHANGING;
        msg.wParam = 0;

        WINDOWPOS  wndpos;
        memset(&wndpos, 0, sizeof(&wndpos));
        wndpos.hwnd = msg.hwnd;
        wndpos.flags = SWP_LAYEREDWINDOW_SIZEMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSENDCHANGING;
        wndpos.x = m_ptWindow.x;
        wndpos.y = m_ptWindow.y;
        wndpos.cx = m_sizeWindow.cx;
        wndpos.cy = m_sizeWindow.cy;
        msg.lParam = (LPARAM)&wndpos;
        ::SendMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
    }
    UpdateLayeredCaptionWindowRgn();
}
void  LayeredWindowWrap::OnEnterSizeMove(unsigned int nHitTest)
{
	HWND hWnd = GetHWND();

    SendMessage(hWnd, WM_ENTERSIZEMOVE, 0, 0);
    SetCapture(hWnd);
    m_nHitTestFlag = nHitTest;

    POINT ptWindow;
    GetCursorPos(&ptWindow);

    m_ptStartSizeMove.x = ptWindow.x;
    m_ptStartSizeMove.y = ptWindow.y;

    m_ptWindowOld.x = m_ptWindow.x;
    m_ptWindowOld.y = m_ptWindow.y;
    m_sizeWindowOld.cx = m_sizeWindow.cx;
    m_sizeWindowOld.cy = m_sizeWindow.cy;
}

void  LayeredWindowWrap::OnExitSizeMove()
{
    if (0 == m_nHitTestFlag)
        return;

    if (GetHWND() == GetCapture())
        ReleaseCapture();

    m_nHitTestFlag = 0;
    m_ptStartSizeMove.x = 0;
    m_ptStartSizeMove.y = 0;

    m_ptWindowOld.x = 0;
    m_ptWindowOld.y = 0;
    m_sizeWindowOld.cx = 0;
    m_sizeWindowOld.cy = 0;
    SendMessage(GetHWND(), WM_EXITSIZEMOVE, 0, 0);
}

// 当窗口最小化了的时候，如果再次用原point/size, Commit，会导致窗口又恢复到最小化之前的位置
// 因此增加一个标志，如果窗口最小化了，则不修改窗口位置，仅在原窗口大小上面提交
bool  LayeredWindowWrap::IsMinimized()
{
    if (m_ptWindow.x == WINDOWS_MINIMIZED_POINT && m_ptWindow.y == WINDOWS_MINIMIZED_POINT)
        return true;

    return false;
};

bool LayeredWindowWrap::Commit()
{
    if (!m_pWindow)
        return false;

	Layer*  pLayer = m_pWindow->GetLayer();
	IRenderTarget* pRenderTarget = pLayer->GetRenderTarget();

	// TBD: 窗口还不可见（ComboBox的listbox），但触发了invalidate操作，会导致update layered window失败
	// 这里先这么处理。
	if (m_sizeWindow.cx <= 0 ||
		m_sizeWindow.cy <= 0)
		return true;

	//POINT ptMemDC  = {rcOffset.left, rcOffset.top};
	POINT ptMemDC = { 0 };

    int   nFlag = ULW_ALPHA/*ULW_OPAQUE*/;
    DWORD dwColorMask = 0;

    BLENDFUNCTION bf;
    bf.BlendOp     = AC_SRC_OVER ;
    bf.AlphaFormat = AC_SRC_ALPHA;        // AC_SRC_ALPHA 会导致窗口被搂空,AC_SRC_OVER不使用透明
    bf.BlendFlags  = 0;                   // Must be zero. 
	bf.SourceConstantAlpha = (byte)m_pWindow->GetLayer()->GetOpacity();         // 0~255 透明度
    // 
    // 	if (bf.SourceConstantAlpha != 255)
    // 	{
    // 		nFlag &= ~ULW_OPAQUE;
    // 		nFlag |= ULW_ALPHA;
    // 	}
    // [注]：在xp、win7不使用桌面主题的情况下，COLOR将有问题，因此建议全部使用PNG来实现透明
    // 	else if (m_pWindow->m_eCurrentTransparentMode & WINDOW_TRANSPARENT_MODE_MASKCOLOR) 
    // 	{
    // 		if (m_pWindow->m_pColMask)
    // 		{
    // 			dwColorMask = m_pWindow->m_pColMask->m_col;
    // 			nFlag = ULW_COLORKEY;
    // 		}
    // 	}

    // 1. 最小化了也要刷新，否则最还原之后会发现画面不连贯了。因为在显示出来的那一瞬间，还是最小化前的画面
    // 2. 当m_sizeWindow大于memDC大小时，会导致绘图失败，并且直接以当前画面的最后一像素并0 alpha进行平铺，平铺面积为memDC大小，但窗口为sizeWindow大小
    BOOL  bRet = ::UpdateLayeredWindow(
        GetHWND(),
		nullptr, 
        IsMinimized() ? nullptr : &m_ptWindow, 
        &m_sizeWindow, 
        pRenderTarget->GetHDC(), // TODO: 考虑d2d提交方式
        &ptMemDC, dwColorMask, &bf, nFlag); 
	UIASSERT(bRet);

    return true;
}


//
// 对于分层窗口，在显示的时候需要重新更新一次。因为当分层窗口隐藏的时候内存位图没有得到更新，invisible false后，直接return了
//
// 注：在这里没有使用响应WM_SHOWWINDOW来通过分层窗口刷新，因为在响应WM_SHOWWINDOW的时候，IsWindowVisible还是FALSE
//     因此改用OnWindowPosChanged来得到窗口显示的时机，通过分层窗口刷新
//   
// long LayeredWindowWrap::_OnWindowPosChanging( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
// {
//     bHandled = FALSE;
//     LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
// 
//     if (m_pLayeredWindowWrap)
//     {
//         m_pLayeredWindowWrap->OnWindowPosChanging(lpWndPos);
//     }
//     return 0;
// }
// void  LayeredWindowWrap::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
// {
//     // 	if (lpWndPos->flags & SWP_SHOWWINDOW)  // 窗口显示（窗口隐藏时，DrawObject会失败）
//     // 	{
//     // 		this->RedrawObject(m_pWindow, TRUE);
//     // 	}
// }

//
//	通知分层窗口新的位置和大小
//
long LayeredWindowWrap::_OnWindowPosChanged(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
	OnWindowPosChanged(lpWndPos);

	return 0;
}

long  LayeredWindowWrap::_OnSize(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	UpdateLayeredCaptionWindowRgn();
    return 0;
}

void  LayeredWindowWrap::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
    // 最小化了，仅保存一下m_ptWindow，用于后面判断是否是最小化，不修改m_sizeWindow（不好处理）
    if (lpWndPos->x == WINDOWS_MINIMIZED_POINT && lpWndPos->y == WINDOWS_MINIMIZED_POINT)
    {
        m_ptWindow.x = WINDOWS_MINIMIZED_POINT;
        m_ptWindow.y = WINDOWS_MINIMIZED_POINT;
    }
    else
    {
        // 这个大小是窗口的大小，不是客户区的大小，因此必须重新调用getclientrect来获取
//         if (!(lpWndPos->flags & SWP_NOMOVE))
//         {
//             m_ptWindow.x = lpWndPos->x;
//             m_ptWindow.y = lpWndPos->y;
//         }
//         if (!(lpWndPos->flags & SWP_NOSIZE))
//         {
//             m_sizeWindow.cx = lpWndPos->cx;
//             m_sizeWindow.cy = lpWndPos->cy;
//         }

        if ((!(lpWndPos->flags & SWP_NOMOVE)) || !(lpWndPos->flags & SWP_NOSIZE))
        {
            GetWindowPos();
        }

        if (lpWndPos->flags & SWP_SHOWWINDOW)  // 窗口显示（窗口隐藏时，DrawObject会失败）
        {
            ::InvalidateRect(GetHWND(), nullptr, TRUE);
        }
    }
}

long LayeredWindowWrap::_OnCancelMode( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;
    OnExitSizeMove();
    return 0;
}

// 分层窗口不使用NC那套拖拽机制，不平滑
long  LayeredWindowWrap::_OnNcHitTest(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return HTCLIENT;
}
long  LayeredWindowWrap::_OnSetCursor(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    POINT pt;
    ::GetCursorPos(&pt);
    ::MapWindowPoints(nullptr, GetHWND(), &pt,1);

    unsigned int nHitTest = m_pWindow->OnHitTest(&pt, nullptr);
    if (nHitTest != HTCLIENT)
    {
        m_pWindow->SetCursorByHitTest(nHitTest);
        return TRUE;
    }
    bHandled = FALSE;
    return 0;
}

//	
//  另外UpdateLayeredWindow支持更平滑的窗口拉伸效果，因此对于分层窗口的拉伸是另外一套逻辑实现的
//
void LayeredWindowWrap::OnLButtonDown(unsigned int nFlags, POINT pt)
{
    unsigned int nHitTest = m_pWindow->OnHitTest(&pt, nullptr);

    switch(nHitTest)
    {
    case HTTOPLEFT:
    case HTTOP:
    case HTTOPRIGHT:
    case HTLEFT:
    case HTRIGHT:
    case HTBOTTOMLEFT:
    case HTBOTTOM:
    case HTBOTTOMRIGHT:
        {
            OnLButtonDown(nHitTest);
            SetMsgHandled(TRUE);
        }
        break;

    case HTCAPTION:
        ::PostMessage(GetHWND(), WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x,pt.y) );
        break;

    default:
        {
            SetMsgHandled(FALSE);
        }
        break;
    }
}

void  LayeredWindowWrap::Enable(bool b)
{
	HWND hWnd = GetHWND();
	UIASSERT(hWnd);

    if (b)
    {
        // 使用分层窗口的话，就不要再显示窗口阴影了。（场景：菜单窗口默认是带SHADOW的，使用分层窗口时就不需要这个阴影了，自己模拟即可）
        SetClassLongPtr(hWnd, GCL_STYLE, GetClassLongPtr(hWnd, GCL_STYLE)&~CS_DROPSHADOW);
        SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

        UpdateLayeredCaptionWindowRgn();
        // SetWindowRgn(hWnd, nullptr, FALSE);   // 取消窗口的异形，由分层窗口自己来处理。分层窗口仅会收到一个WINDOWPOSCHANGED消息，但SWP_NOSIZE，
        // 因此还需要另外发送一个窗口大小的消息告诉分层窗口当前大小
    }
    else
    {
        // 注：直接去除WS_EX_LAYERED属性，会导致窗口先变黑，然后再刷新一次。
        //     因此在这里去除分层属性之后，直接将mem bitmap的内容先画到窗口上来规避这个问题。
        //     同时为了解决画到窗口上的内容有rgn以外的部分，在去除分层属性之前，先计算
        //     窗口的形状

        m_pWindow->UpdateWindowRgn();
        SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

        HDC hDC = ::GetDC(hWnd);
        RECT  rc = {0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight()};
        m_pWindow->BitBltMemBitmap(hDC, &rc);
        ReleaseDC(hWnd, hDC);
    }

	// 取消窗口异形  TEST IT!
	if (b)
	{
		SetWindowRgn(hWnd, nullptr, FALSE);
	}
}


ILayeredWindowWrap*  LayeredWindowWrap::GetILayeredWindowWrap()
{
    if (!m_pILayeredWindowWrap)
        m_pILayeredWindowWrap = new ILayeredWindowWrap(this);

    return m_pILayeredWindowWrap;
}

void  LayeredWindowWrap::UpdateRgn()
{
	// 尝试修改为仅在初始化的时候设置一次。
//     HWND hWnd = GetHWND();
//     RECT rc;
//     GetWindowRect(hWnd, &rc);
//     ::OffsetRect(&rc, -rc.left, -rc.top);
// 
//     HRGN hRgn = CreateRectRgnIndirect(&rc);
//     SetWindowRgn(hWnd, hRgn, TRUE);
}

bool  LayeredWindowWrap::UpdateLayeredCaptionWindowRgn()
{
	HWND hWnd = GetHWND();
    if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_CAPTION)
    {
        // SetWindowRgn(nullptr)不管用，依然会有标题栏的圆角
        RECT rc;
        GetWindowRect(hWnd, &rc);
        ::OffsetRect(&rc, -rc.left, -rc.top);

       HRGN hRgn = CreateRectRgnIndirect(&rc);
       SetWindowRgn(hWnd, hRgn, TRUE);
        // SAFE_DELETE_GDIOBJECT(hRgn);   // TODO: MSDN上表示不需要销毁该对象
       return true;
    }

    return false;
}
}