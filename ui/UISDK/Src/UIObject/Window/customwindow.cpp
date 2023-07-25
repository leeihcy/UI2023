#include "stdafx.h"
#include "customwindow.h"
#include "src/Renderbase\renderbase\renderbase.h"
#include "include/interface/imapattr.h"
#include "src/UIObject\Window\wndtransmode\layered\layeredwrap.h"
#include "src/UIObject\Window\wndtransmode\aero\aerowrap.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "window_desc.h"
#include "include/interface/ilayout.h"

CustomWindow::CustomWindow(ICustomWindow* p) : Window(p)
{
	m_pICustomWindow = p;
    m_pWindowTransparent = nullptr;
	m_bNeedToSetWindowRgn = true;   // 首次显示时，需要设置一下窗口形状
    m_bFirstNcCalcSize = true;
    m_bRemoveMAXBOX = false;

	m_lResizeBorder = 6;
    m_lMaximizeBorder = 3;
	m_lResizeCapability = WRSB_CAPTION;
    m_bAeroCaptionAnimate = true;

	SetDescription(CustomWindowDescription::Get());
}
CustomWindow::~CustomWindow()
{
	SAFE_RELEASE(m_pWindowTransparent);
}

BOOL  CustomWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    if (FALSE == GetCurMsg()->lRet)
        return FALSE;

	if (!(pcs->style & WS_CHILD))
	{
		pcs->style = DS_SETFONT | WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN;

        if (m_bAeroCaptionAnimate && DwmHelper::GetInstance()->IsEnable())
        {
            pcs->style |= WS_CAPTION;
        }
	}
	return TRUE;
}

void CustomWindow::virtualInnerInitWindow( )
{
	__super::virtualInnerInitWindow();

    // 属性加载时，创建了m_pWindowTransparent，但窗口那时还没有创建，
    // 因此在这里进行初始化
	if (m_pWindowTransparent)
        SetWindowTransparent(m_pWindowTransparent);
	
	LONG_PTR dwStyleEx = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE );
	dwStyleEx &= ~ WS_EX_WINDOWEDGE;
	SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, dwStyleEx );
    
    // 没有WS_THICKFRAME的自绘窗口，有几个缺陷：
    // 1. 系统菜单中的"大小"不可用
    // 2. 不支持win7下的窗口贴边最大化效果
    // 3. HitTest返回HTTOP等时不会自动拉伸
    //
    // 但是给分层窗口增加THICKFRAME属性会导致客户区域与窗口区域不一致的问题
    // 最终 updatelayeredwindow的ptwnd参数有歧义！
    //

	LONG_PTR dwStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
    if ((m_lResizeCapability & WRSB_DRAGSIZE) && 
        !(dwStyle & WS_THICKFRAME))
    {
         dwStyle |= WS_THICKFRAME;
         SetWindowLongPtr(m_hWnd, GWL_STYLE, dwStyle);
    }
}

void  CustomWindow::OnSerialize(SERIALIZEDATA* pData)
{
	// 注：这里需要将设置分层窗口的属性放在前面。因为object中很多背景图依赖于窗口类型
	Window::OnSerialize(pData);

    if (pData->IsReload())
    {
        m_bNeedToSetWindowRgn = true; // 换肤时，重新更新窗口形状

        if (m_pWindowTransparent)
        {
            m_pWindowTransparent->Enable(false);
            SAFE_RELEASE(m_pWindowTransparent);
        }
    }

    AttributeSerializer s(pData, TEXT("CustomWindow"));

    s.AddLong(XML_WINDOW_RESIZE_BORDER, m_lResizeBorder);
    s.AddLong(XML_WINDOW_MAXIMIZE_BORDER, m_lMaximizeBorder);

    s.AddEnum(XML_WINDOW_RESIZE_CAPABILITY, this,
        memfun_cast<pfnLongSetter>(&CustomWindow::SetResizeCapability),
        memfun_cast<pfnLongGetter>(&CustomWindow::GetResizeCapability))
        ->AddOption(WRSB_ALL,     XML_WINDOW_RESIZE_CAPABILITY_ALL)
        ->AddOption(WRSB_CAPTION, XML_WINDOW_RESIZE_CAPABILITY_CAPTION)
		->AddOption(WRSB_TRANSPARENT, XML_WINDOW_RESIZE_CAPABILITY_TRANSPARENT)
        ->AddOption(WRSB_NONE,    XML_WINDOW_RESIZE_CAPABILITY_NONE)
		->SetDefault(WRSB_CAPTION);
    
    s.AddEnum(XML_WINDOW_TRANSPARENT_TYPE, this,
        memfun_cast<pfnLongSetter>(&CustomWindow::SetWindowTransparentType),
        memfun_cast<pfnLongGetter>(&CustomWindow::GetWindowTransparentType))
        ->AddOption(WINDOW_TRANSPARENT_TYPE_NORMAL,       L"")
        ->AddOption(WINDOW_TRANSPARENT_TYPE_LAYERED,      XML_WINDOW_TRANSPARENT_TYPE_LAYERED)
        ->AddOption(WINDOW_TRANSPARENT_TYPE_AERO,         XML_WINDOW_TRANSPARENT_TYPE_AERO)
		->AddOption(WINDOW_TRANSPARENT_TYPE_GLOW,         XML_WINDOW_TRANSPARENT_TYPE_GLOW)
        ->AddOption(WINDOW_TRANSPARENT_TYPE_MASKALPHA,    XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA)
        ->AddOption(WINDOW_TRANSPARENT_TYPE_MASKCOLOR,    XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR)
        ->AddOption(WINDOW_TRANSPARENT_TYPE_ANTIALIASING, XML_WINDOW_TRANSPARENT_TYPE_ANTIALIASING)
        ->SetDefault(WINDOW_TRANSPARENT_TYPE_NORMAL);
}

void  CustomWindow::virtualOnPostDrawObjectErasebkgnd()
{
	if (m_bNeedToSetWindowRgn)
	{
		// 重新设置窗口透明形状 
		// 注：
		//    1. 不将该段代码放在OnErasebkgnd中的原因是，刷新一个按钮时也会走到
		// OnEraseBkgnd中，导致这时的背景图片中被剪裁只剩下一个控件，update window rgn错误。
		//    2. 同理，也不能放在CommitDoubleBuffet2Window中，因为也有可能是初始中刷新了一个对象
		//    3. 如果放在_OnPaint当中的话，则会将窗口上的文字由于采用GDI绘制alpha变成0，也会被抠掉

		this->UpdateWindowRgn();
	}
}


// Called from OnNCActivate.
BOOL CALLBACK EnumChildWindowsForRedraw(HWND hwnd, LPARAM lparam) 
{
    DWORD process_id;
    GetWindowThreadProcessId(hwnd, &process_id);
    int flags = RDW_INVALIDATE | RDW_NOCHILDREN | RDW_FRAME;
    if (process_id == GetCurrentProcessId())
        flags |= RDW_UPDATENOW;
    RedrawWindow(hwnd, nullptr, nullptr, flags);
    return TRUE;
}
//
// 屏蔽WM_NCACTIVATE消息，否则在win7上被激活或失活会有边框(但是因为现在不使用WS_THICKFRAME来实现拖拽，也没有这个
// 问题了，这里响应该消息仅为了实现窗口刷新）
// 20121127 后注:强制返回1将导致从一个模态customwindow中再弹出一个模态窗口时（如CFileDialog），新弹出的窗口将永远不能被激活。
//               貌似是子模态窗口会向父窗口发送这个消息进行检测
//
// Remark
//	Return Nonzero if Windows should proceed with default processing; 0 to prevent the caption bar or icon from being deactivated.
//
long CustomWindow::_OnNcActivate(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    this->SetActive(wParam?true:false);

	// 注:不调用这句会导致窗口显示的时候出现标题栏
	// Hack to redraw this window and child windows synchronously upon activation. 
	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_NOCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW);

	if (::GetWindow(m_hWnd, GW_CHILD))
	{
		EnumChildWindows(m_hWnd, EnumChildWindowsForRedraw, 0);
	}

    bHandled = TRUE;
    return 1;
}


// 解决在经典主题下面，调用setwindowtext/seticon会导致标题栏被系统重绘
long  CustomWindow::_OnSetTextIcon(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (m_hWnd && IsWindowVisible())
    {
		LONG_PTR lOldStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
        SetWindowLongPtr(m_hWnd, GWL_STYLE, lOldStyle &~ WS_CAPTION);
        long lRet = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        SetWindowLongPtr(m_hWnd, GWL_STYLE, lOldStyle);
        return lRet;
    }
    else
    {
        bHandled = FALSE;
        return 0;
    }
}
long  CustomWindow::_OnNCUAHDrawCaption(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;
    return 0;
}
long  CustomWindow::_OnNCUAHDrawFrame(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;
    return 0;
}

long  CustomWindow::_OnDwmCompositionChanged(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // Win7下面，开启Aero之后，只有带WS_CAPTION的窗口才能使用系统自带动画
    // 在切换到经典模式之后，窗口最大化/还原时，点击下标题栏区域，会显示系统
    // 自带的标题栏，原因未知。因此在非aero模式下去掉WS_CAPTION
    if (m_bAeroCaptionAnimate)
    {
        bool bDWMEnable = DwmHelper::GetInstance()->IsEnable();
        bool bCaptionStyle = TESTWNDSTYLE(m_hWnd, WS_CAPTION) ? true:false;

        if (bDWMEnable && !bCaptionStyle)
        {
            ADDWNDSTYLE(m_hWnd, WS_CAPTION);
        }
        else if (!bDWMEnable && bCaptionStyle)
        {
            REMOVEWNDSTYLE(m_hWnd, WS_CAPTION);
        }
    }
    bHandled = FALSE;
    return 0;
}

// 注：带WS_CAPTION样式的无边框窗口，最大化后的区域在windowposchanging中修改无效，
//     在WM_NCCALCSIZE中能够看到，又被系统强制修改为边框大小
//
//     响应顺序：
//          WM_GETMINMAXINFO -> WindowPoschanging -> NcCalcSize -> WindowPosChanged
//
//  swp.c xxxCheckFullScreen:
//
//  如果最大化时，WindowPos大于当前屏幕的workarea：
//
//   . 多屏下面，如果不是最大化到主屏，则将wndpos作偏移
//
//         /*
//          * Transfer over the shape to the REAL monitor.
//          */
//         psrc->x += pMonitorReal->rcMonitor.left;
//         psrc->y  += pMonitorReal->rcMonitor.top;
//         psrc->cx -= (pMonitor->rcMonitor.right - pMonitor->rcMonitor.left) +
//             (pMonitorReal->rcMonitor.right - pMonitorReal->rcMonitor.left);
//         psrc->cy -= (pMonitor->rcMonitor.bottom - pMonitor->rcMonitor.top) +
//             (pMonitorReal->rcMonitor.bottom - pMonitorReal->rcMonitor.top);
//
//   . 最大化下面，如果窗口有最大化属性、标题栏属性(MAXIMIZEDBOX,CAPTION)
//     - 如果窗口客户区域大于屏幕区域，表示该窗口是真正想全屏，不处理：
//        if (psrc->y + SYSMET(CYCAPTION) <= pMonitor->rcMonitor.top &&
//            psrc->y + psrc->cy >= pMonitor->rcMonitor.bottom) 
//        {
//            if (!TestWF(pwnd, WFFULLSCREEN)) 
//            {
//                /*
//                 * Only want to do full screen stuff on the tray
//                 * monitor.
//                 */
//                fYielded = xxxAddFullScreen(pwnd, pMonitor);
//            }
//        } 
//
//     - 否则该窗口是真的想最大化，对其最大化区域进行修改，完全无视了
//       WM_GETMINMAXINFO/WM_WindowPosChanging的返回值
//
//         /*
//          * Despite the code in GetMinMaxInfo() to fix up
//          * the max rect, we still have to hack old apps.
//          */
//         dxy = GetWindowBorders(pwnd->style, pwnd->ExStyle, TRUE, FALSE);
//         dxy *= SYSMET(CXBORDER);
// 
//         psrc->x = pMonitor->rcWork.left - dxy;
//         psrc->y = pMonitor->rcWork.top - dxy;
// 
//         dxy *= 2;
//         iRight =
//         pMonitor->rcWork.right - pMonitor->rcWork.left + dxy;
//         iBottom =
//         pMonitor->rcWork.bottom - pMonitor->rcWork.top + dxy;

        /*
         * Let console windows maximize smaller than defaults.
         */
//         if (pwnd->pcls->atomClassName == gatomConsoleClass) 
//         {
//             psrc->cx = min(iRight, psrc->cx);
//             psrc->cy = min(iBottom, psrc->cy);
//         } 
//         else 
//         {
//             psrc->cx = iRight;
// 
//             if (fIsPrimary && !TestWF(pwnd, WFWIN40COMPAT)) {
//                 psrc->cy = min(iBottom, psrc->cy);
//             } else {
//                 psrc->cy = iBottom;
//             }
//         }
//
//
long  CustomWindow::_OnWindowPosChanging(
            unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    __super::_OnWindowPosChanging(uMsg, wParam, lParam, bHandled);
    LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam;

     // 第一次最大化，而不是最大化之后所触发的WINDOWPOSCHANGE
    if (IsZoomed(m_hWnd) && lpPos->flags & SWP_FRAMECHANGED)
    {
        // 1. 解决无边框(WS_CAPTION|WS_MAXIMIZEDBOX)窗口最大化后覆盖任务栏的问题
        // 2. 解决有边框窗口最大化后，会被系统强制修改为固定大小的问题

        RECT rcWndPos = {0};
        if (!(lpPos->flags&SWP_NOMOVE))
        {
            rcWndPos.left = lpPos->x;
            rcWndPos.top = lpPos->y;
        }
        if (!(lpPos->flags&SWP_NOSIZE))
        {
            rcWndPos.right = rcWndPos.left + lpPos->cx;
            rcWndPos.bottom = rcWndPos.top + lpPos->cy;
        }

        // 最大化，然后最小化，此时再还原，调用MonitorFromWindow拿到的是主屏幕，而不是最大化时所在屏幕
        //MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);  ，
        HMONITOR  hMonitorTo = MonitorFromRect(&rcWndPos, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO  miTo = {sizeof(miTo), 0};

        CRect rcWorkArea;
        if (GetMonitorInfo(hMonitorTo, &miTo))
        {
            rcWorkArea = miTo.rcWork;
        }
        else
        {
            ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
        }

        lpPos->x = rcWorkArea.left;
        lpPos->y = rcWorkArea.top;
        lpPos->cx = rcWorkArea.right-rcWorkArea.left;
        lpPos->cy = rcWorkArea.bottom-rcWorkArea.top;

        lpPos->x -= m_lMaximizeBorder;
        lpPos->y -= m_lMaximizeBorder;
        lpPos->cx += m_lMaximizeBorder*2;
        lpPos->cy += m_lMaximizeBorder*2;

        if (TESTWNDSTYLE(m_hWnd, WS_MAXIMIZEBOX))
        {
            REMOVEWNDSTYLE(m_hWnd, WS_MAXIMIZEBOX);
            m_bRemoveMAXBOX = true;
        }
    }

    return 0;
}

// 不显示标题和边框
long  CustomWindow::_OnNcCalcSize(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // chrome: 在任务栏上右击，选择层叠等无反应
    // Let User32 handle the first nccalcsize for captioned windows
    // so it updates its internal structures (specifically caption-present)
    // Without this Tile & Cascade windows won't work.
    if (m_bFirstNcCalcSize)
    { 
        m_bFirstNcCalcSize = false;
        bHandled = FALSE;
        return 0;
    }

    if (IsZoomed(m_hWnd))
    {
        // 在WM_WINDOWPOSCHANGING中去掉了最大化样式，在这一步加回去
        if (m_bRemoveMAXBOX)
        {
            m_bRemoveMAXBOX = false;
            ADDWNDSTYLE(m_hWnd, WS_MAXIMIZEBOX);
        }
    }

//     if (m_pWindowTransparent)
//     {
//         long lRet = 0;
//         m_pWindowTransparent->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, lRet, 0);
//     }
    bHandled = TRUE;
    return 0;
}

long  CustomWindow::_OnNcHitTest(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    MapWindowPoints(nullptr, m_hWnd, &pt, 1);
    return this->OnHitTest(&pt, nullptr);
}

// 增加最大化时对m_nMaximizeBorder的处理。
// 默认操作系统在窗口最大化时，只允许窗口最大为屏幕大小+边框大小。
// 但如果一个自定义窗口的m_nMaximizeBorder很大(如100，超过边框大小）时，最大化后窗口的max值就不正确了。
// 因此在这里对这种情况进行处理。（多屏下面允许的窗口最大值为多个屏幕的总宽度，
// 一般没什么问题，只需要处理单屏即可）
long CustomWindow::_OnGetMinMaxInfo( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;  // 由windowbase继续处理

    if (NDEF == m_nMaxWidth || NDEF == m_nMaxHeight)
    {
        MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
        int  nMonitorCount = GetSystemMetrics(SM_CMONITORS);
        if (1 == nMonitorCount)
        {
            // 有些窗口要全屏，因此最大范围应该是整个屏幕，而不是工作区域
//          CRect  rcWorkArea;
//         ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

            if (NDEF == m_nMaxWidth)
            {
                int nDesiredMaxWidth = GetSystemMetrics(SM_CXSCREEN) + m_lMaximizeBorder*2;
                pInfo->ptMaxSize.x = pInfo->ptMaxTrackSize.x = nDesiredMaxWidth;
            }

            if (NDEF == m_nMaxHeight)
            {
                int nDesiredMaxHeight = GetSystemMetrics(SM_CYSCREEN) + m_lMaximizeBorder*2;
                pInfo->ptMaxSize.y = pInfo->ptMaxTrackSize.y = nDesiredMaxHeight;
            }
        }
    }

    return 0;
}

// long CustomWindow::_OnNcPaint( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
// {
// 	bHandled = FALSE;
// 	return 0;
// }

void CustomWindow::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	if (m_pBkgndRender || m_pForegndRender || m_pTextureRender)
	{
		if (m_pLayout && m_pLayout->IsDirty())
		{
			m_pLayout->Arrange(nullptr);
		}

		CRect rc(0,0, GetWidth(), GetHeight());
		if (rc.IsRectEmpty())
			return;

		int nState = this->IsActive()?WINDOW_BKGND_RENDER_STATE_ACTIVE:WINDOW_BKGND_RENDER_STATE_INACTIVE;

        if (m_pBkgndRender)
        {
            CRect rcBkgnd(&rc);
            rcBkgnd.DeflateRect(&m_rcBkgndRenderRegion);
            m_pBkgndRender->DrawState(pRenderTarget, &rcBkgnd, nState);
        }

        if (m_pTextureRender)
        {
            CRect rcTextureRegion(&rc);
            rcTextureRegion.DeflateRect(&m_rcTextureRenderRegion);
            m_pTextureRender->DrawState(pRenderTarget, &rcTextureRegion, nState);
        }

        if (m_pForegndRender)
        {
            CRect rcForegnd(&rc);
            rcForegnd.DeflateRect(&m_rcForegndRenderRegion);
            m_pForegndRender->DrawState(pRenderTarget, &rcForegnd, nState);
        }
	}
	else
	{
		__super::OnEraseBkgnd(pRenderTarget);
	}
}

void CustomWindow::UpdateWindowRgn()
{
	this->m_bNeedToSetWindowRgn = false;

    if (m_pWindowTransparent)
        m_pWindowTransparent->UpdateRgn();
}


bool  CustomWindow::virtualCommitReq()
{
	if (m_pWindowTransparent && m_pWindowTransparent->Commit())
        return true;
    
	return false;
}

bool IsPanel(Object* p)
{
	if (!p)
		return false;

	if (!p->GetDescription())
		return false;

	if (p->GetDescription()->GetMajorType() != OBJ_PANEL)
		return false;

	return true;
}
//
//	获取一个POINT在CustomWindow上面的位置
//
int CustomWindow::OnHitTest(POINT* pt, POINT*  ptInChild)
{
	if (m_lResizeCapability == WRSB_NONE)
	{
		return HTCLIENT;
	}
 	else if (m_lResizeCapability == WRSB_CAPTION)
	{
		// TBD: panel 不处理hittest
		Object* pObjPress = GetPressObject();
		if (IsPanel(pObjPress))
			pObjPress = nullptr;
		
		if (nullptr == pObjPress && !IsZoomed(m_hWnd))
        {
			// 这个时候hover obj还没有更新
            Object* pObj = GetObjectByPos(this, pt);

			// TBD: panel 不处理hittest
			if (IsPanel(pObj))
				pObj = nullptr;

            if (!pObj)
			    return HTCAPTION;
        }
	}
	else if (m_lResizeCapability == WRSB_TRANSPARENT)
	{
		// TBD: panel 不处理hittest
		Object* pObjPress = GetPressObject();
		if (IsPanel(pObjPress))
			pObjPress = nullptr;

		if (nullptr == pObjPress)
		{
			// 这个时候hover obj还没有更新
			Object* pObj = GetObjectByPos(this, pt);

			// TBD: panel 不处理hittest
			if (IsPanel(pObj))
				pObj = nullptr;

			if (!pObj)
			{
// 				HWND hWndParent = GetParent(m_hWnd);
// 				if (hWndParent)
				{ 
					return HTTRANSPARENT;
				}
			}
		}
	}

	if (nullptr == pt)
		return HTERROR;

	unsigned int nHitTest = HTCLIENT;
	if (IsZoomed(m_hWnd))
    {
        // Win7下面最大化时，允许拖拽还原窗口
        if (m_lResizeCapability & WRSB_CAPTION)
        {
            Object* pObj = GetObjectByPos(this, pt);
            if (!pObj)
                return HTCAPTION;
        }

		return HTCLIENT;
    }

	RECT rc;
	::GetWindowRect(m_hWnd, &rc);
    ::OffsetRect(&rc, -rc.left, -rc.top);
    if (!PtInRect(&rc, *pt))
        return HTNOWHERE;

	int nWidth  = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;

	if (pt->x < m_lResizeBorder)
	{
		if (pt->y < m_lResizeBorder && this->TestResizeBit(WRSB_TOPLEFT))
			nHitTest = HTTOPLEFT;
		else if (pt->y > nHeight - m_lResizeBorder  && this->TestResizeBit(WRSB_BOTTOMLEFT))
			nHitTest = HTBOTTOMLEFT;
		else if (this->TestResizeBit(WRSB_LEFT))
			nHitTest = HTLEFT;
	}
	else if (pt->x >= nWidth-m_lResizeBorder)
	{
		if (pt->y < m_lResizeBorder && this->TestResizeBit(WRSB_TOPRIGHT))
			nHitTest = HTTOPRIGHT;
		else if (pt->y > nHeight - m_lResizeBorder && this->TestResizeBit(WRSB_BOTTOMRIGHT))
			nHitTest = HTBOTTOMRIGHT;
		else if (this->TestResizeBit(WRSB_RIGHT))
			nHitTest = HTRIGHT;
	}
	else if (pt->y < m_lResizeBorder && this->TestResizeBit(WRSB_TOP))
	{
		nHitTest = HTTOP;
	}
	else if (pt->y >= nHeight - m_lResizeBorder && this->TestResizeBit(WRSB_BOTTOM))
	{
		nHitTest = HTBOTTOM;
	}
 	else
 	{
 		if (m_lResizeCapability & WRSB_CAPTION && !IsZoomed(m_hWnd))
 		{
            Object* pObj = GetObjectByPos(this, pt);
            if (!pObj)
 			    nHitTest = HTCAPTION;
 		}
 	}

	return nHitTest;

}

//
//	使用通过在 NCHitTest中返回HTCAPTION也能使用点击拖拽的效果，但会产生一个WM_MOUSELEAVE消息，因为鼠标不在客户区域了
//
//	使用WS_THICKFRAME也可以直接实现边框拖拽效果，但还需要去响应NCCACLSIZE和添加一个属性，可能会导致窗口在WIN7下面显示透明边框
//	
//  另外UpdateLayeredWindow支持更平滑的窗口拉伸效果，因此对于分层窗口的拉伸是另外一套逻辑实现的
//
// void CustomWindow::OnLButtonDown(unsigned int nFlags, POINT pt)
// {
// 	SetMsgHandled(FALSE);
// 
// 	unsigned int nHitTest = this->OnHitTest(&pt, nullptr);
// 	switch(nHitTest)
// 	{
// 	case HTTOPLEFT:
// 		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPLEFT, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTTOP:
// 		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOP, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTTOPRIGHT:
// 		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPRIGHT, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTLEFT:
// 		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_LEFT, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTRIGHT:
// 		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_RIGHT, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTBOTTOMLEFT:
// 		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMLEFT, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTBOTTOM:
// 		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOM, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTBOTTOMRIGHT:
// 	    ::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMRIGHT, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 
// 	case HTCAPTION:
// 		::PostMessage( m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x,pt.y) );
// 		break;
// 	}
// }

// TODO: (没解决的了，下次再看）
// 用于解决没有标题栏的窗口，最小化和还原的时候，没有动画的问题。
// 没有动画也会导致窗口还原时造成闪烁(例如使用了aero效果时)
void  CustomWindow::OnSysCommand(unsigned int nID, CPoint point)
{
//     if (nID == SC_MINIMIZE || (nID == SC_RESTORE && IsIconic(m_hWnd)))
//     {
//         long lStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
//         bool bHasCaption =  lStyle & WS_CAPTION ? true:false;
//         if (!bHasCaption)
//         {
//             //SetWindowLongPtr(m_hWnd, GWL_STYLE, lStyle | WS_CAPTION);
//             ::DefWindowProc(m_hWnd, WM_SYSCOMMAND, GetCurMsg()->wParam, GetCurMsg()->lParam);
//             //SetWindowLongPtr(m_hWnd, GWL_STYLE, lStyle);
//             return;
//         }
//     }

    SetMsgHandled(FALSE);
}

void  CustomWindow::OnLButtonDblClk(unsigned int nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	if (::IsZoomed(m_hWnd))
	{
		::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else
	{
		if (WS_MAXIMIZEBOX & GetWindowLongPtr(m_hWnd, GWL_STYLE))
		{
			::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
	}
}

// 注：不要响应UIMSG的WM_SIZE。因为在WindowBase::_OnSize中就已经开始更新窗口了，因
//     此需要在那之前将m_bNeedToSetWindowRgn标志置上。否则将光置上标志，却错过了OnEndEraseBknd
void  CustomWindow::virtualOnSize(unsigned int nType, unsigned int nWidth, unsigned int nHeight)
{
    __super::virtualOnSize(nType, nWidth, nHeight);

	// 最小化和从最小化还原时，不需要重新计算窗口的形状
	if (SIZE_MINIMIZED == nType || (SIZE_RESTORED==nType && ::IsIconic(m_hWnd)))
		return;

	this->m_bNeedToSetWindowRgn = true;
}

//
//	设置窗口是否可以拖拽
//
void CustomWindow::SetResizeCapability(long nType)
{
	m_lResizeCapability = nType;

	LONG_PTR dwStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
    if ((m_lResizeCapability & WRSB_DRAGSIZE) && !(dwStyle & WS_THICKFRAME))
    {
         dwStyle |= WS_THICKFRAME;
         SetWindowLongPtr(m_hWnd, GWL_STYLE, dwStyle);
    }
}
long  CustomWindow::GetResizeCapability()
{
    return m_lResizeCapability;
}

//
// 判断窗口的拉伸状态位
//
bool  CustomWindow::TestResizeBit( int nBit )
{
	if (m_lResizeCapability & nBit)
		return true;
	else 
		return false;
}


bool  CustomWindow::IsWindowLayered()
{
    if (m_pWindowTransparent && WINDOW_TRANSPARENT_TYPE_LAYERED == m_pWindowTransparent->GetType())
        return true;

    return false;
}


void  CustomWindow::SetWindowTransparent(IWindowTransparent* pMode)
{
    IWindowTransparent* pOldMode = m_pWindowTransparent;
    m_pWindowTransparent = pMode;

    // 如果这次的赋值与现有值一致，允许调用
    if (pOldMode == pMode)
        pOldMode = nullptr;


    // 可能是创建窗口时属性序列化调用的，这时还没有创建窗口
    // 在窗口初始时再调用一次
    if (!m_hWnd)  
        return;

    if (pMode)
    {
        pMode->Init(m_pICustomWindow);
        pMode->Enable(true);
    }
    m_bNeedToSetWindowRgn = true;
    if (pOldMode)
    {
        pOldMode->Enable(false);  // 注：对于分层窗口的disable，需要先让当前的trans mode设置好它的rgn后，才能取消WS_EX_LAYERED样式，否则会导致窗口变黑
        SAFE_RELEASE(pOldMode);
    }

    //    m_pRenderChain->GetWindowLayer()->ReCreateRenderTarget();

    // 每次是否需要清空缓存，避免alpha叠加
    WINDOW_TRANSPARENT_TYPE eMode = GetWindowTransparentType();
    if (eMode == WINDOW_TRANSPARENT_TYPE_LAYERED || 
        eMode == WINDOW_TRANSPARENT_TYPE_AERO)
        this->SetTransparent(true); 
    else
        this->SetTransparent(false);
}

IWindowTransparent*  CustomWindow::GetWindowTransparentPtr()
{
    return m_pWindowTransparent;
}

void  CustomWindow::SetWindowTransparentType(WINDOW_TRANSPARENT_TYPE eMode)
{
    IWindowTransparent*  pMode = CreateTransparentModeByType(eMode);
    if (pMode)
    {
        SetWindowTransparent(pMode);
    }
}

WINDOW_TRANSPARENT_TYPE  CustomWindow::GetWindowTransparentType()
{
    if (m_pWindowTransparent)
        return m_pWindowTransparent->GetType();
    else
        return WINDOW_TRANSPARENT_TYPE_NORMAL;
}
//
//	设置/取消一个窗口为分层窗口
//
void  CustomWindow::EnableWindowLayered(bool b)
{
    bool bLayered = IsWindowLayered();
	if (b == bLayered)
		return;

	if (b)
	{
		IWindowTransparent* pMode = static_cast<IWindowTransparent*>(new LayeredWindowWrap());
        SetWindowTransparent(pMode);
	}
	else
	{
        m_pWindowTransparent->Enable(false);
        SetWindowTransparent(nullptr);
	}
}

void  CustomWindow::EnableWindowAero(bool b)
{
    bool bAero = (m_pWindowTransparent && WINDOW_TRANSPARENT_TYPE_AERO == m_pWindowTransparent->GetType());
    if (b == bAero)
        return;

    if (b)
    {
        IWindowTransparent* pMode = static_cast<IWindowTransparent*>(new AeroWindowWrap());
        SetWindowTransparent(pMode);
    }
    else
    {
        SetWindowTransparent(nullptr);
    }
}

// WS_CAPTION|WS_MAXIMIZEBOX的窗口最大化后，才不会遮住任务栏
bool  CustomWindow::IsBorderlessWindow()
{
	LONG_PTR lStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
    if (lStyle & WS_CAPTION)
        return false;

    return true;
}