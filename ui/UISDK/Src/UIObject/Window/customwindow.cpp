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
	m_bNeedToSetWindowRgn = true;   // �״���ʾʱ����Ҫ����һ�´�����״
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

    // ���Լ���ʱ��������m_pWindowTransparent����������ʱ��û�д�����
    // �����������г�ʼ��
	if (m_pWindowTransparent)
        SetWindowTransparent(m_pWindowTransparent);
	
	LONG_PTR dwStyleEx = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE );
	dwStyleEx &= ~ WS_EX_WINDOWEDGE;
	SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, dwStyleEx );
    
    // û��WS_THICKFRAME���Ի洰�ڣ��м���ȱ�ݣ�
    // 1. ϵͳ�˵��е�"��С"������
    // 2. ��֧��win7�µĴ����������Ч��
    // 3. HitTest����HTTOP��ʱ�����Զ�����
    //
    // ���Ǹ��ֲ㴰������THICKFRAME���Իᵼ�¿ͻ������봰������һ�µ�����
    // ���� updatelayeredwindow��ptwnd���������壡
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
	// ע��������Ҫ�����÷ֲ㴰�ڵ����Է���ǰ�档��Ϊobject�кܶ౳��ͼ�����ڴ�������
	Window::OnSerialize(pData);

    if (pData->IsReload())
    {
        m_bNeedToSetWindowRgn = true; // ����ʱ�����¸��´�����״

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
		// �������ô���͸����״ 
		// ע��
		//    1. �����öδ������OnErasebkgnd�е�ԭ���ǣ�ˢ��һ����ťʱҲ���ߵ�
		// OnEraseBkgnd�У�������ʱ�ı���ͼƬ�б�����ֻʣ��һ���ؼ���update window rgn����
		//    2. ͬ��Ҳ���ܷ���CommitDoubleBuffet2Window�У���ΪҲ�п����ǳ�ʼ��ˢ����һ������
		//    3. �������_OnPaint���еĻ�����Ὣ�����ϵ��������ڲ���GDI����alpha���0��Ҳ�ᱻ�ٵ�

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
// ����WM_NCACTIVATE��Ϣ��������win7�ϱ������ʧ����б߿�(������Ϊ���ڲ�ʹ��WS_THICKFRAME��ʵ����ק��Ҳû�����
// �����ˣ�������Ӧ����Ϣ��Ϊ��ʵ�ִ���ˢ�£�
// 20121127 ��ע:ǿ�Ʒ���1�����´�һ��ģ̬customwindow���ٵ���һ��ģ̬����ʱ����CFileDialog�����µ����Ĵ��ڽ���Զ���ܱ����
//               ò������ģ̬���ڻ��򸸴��ڷ��������Ϣ���м��
//
// Remark
//	Return Nonzero if Windows should proceed with default processing; 0 to prevent the caption bar or icon from being deactivated.
//
long CustomWindow::_OnNcActivate(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    this->SetActive(wParam?true:false);

	// ע:���������ᵼ�´�����ʾ��ʱ����ֱ�����
	// Hack to redraw this window and child windows synchronously upon activation. 
	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_NOCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW);

	if (::GetWindow(m_hWnd, GW_CHILD))
	{
		EnumChildWindows(m_hWnd, EnumChildWindowsForRedraw, 0);
	}

    bHandled = TRUE;
    return 1;
}


// ����ھ����������棬����setwindowtext/seticon�ᵼ�±�������ϵͳ�ػ�
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
    // Win7���棬����Aero֮��ֻ�д�WS_CAPTION�Ĵ��ڲ���ʹ��ϵͳ�Դ�����
    // ���л�������ģʽ֮�󣬴������/��ԭʱ������±��������򣬻���ʾϵͳ
    // �Դ��ı�������ԭ��δ֪������ڷ�aeroģʽ��ȥ��WS_CAPTION
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

// ע����WS_CAPTION��ʽ���ޱ߿򴰿ڣ���󻯺��������windowposchanging���޸���Ч��
//     ��WM_NCCALCSIZE���ܹ��������ֱ�ϵͳǿ���޸�Ϊ�߿��С
//
//     ��Ӧ˳��
//          WM_GETMINMAXINFO -> WindowPoschanging -> NcCalcSize -> WindowPosChanged
//
//  swp.c xxxCheckFullScreen:
//
//  ������ʱ��WindowPos���ڵ�ǰ��Ļ��workarea��
//
//   . �������棬���������󻯵���������wndpos��ƫ��
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
//   . ������棬���������������ԡ�����������(MAXIMIZEDBOX,CAPTION)
//     - ������ڿͻ����������Ļ���򣬱�ʾ�ô�����������ȫ����������
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
//     - ����ô������������󻯣����������������޸ģ���ȫ������
//       WM_GETMINMAXINFO/WM_WindowPosChanging�ķ���ֵ
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

     // ��һ����󻯣����������֮����������WINDOWPOSCHANGE
    if (IsZoomed(m_hWnd) && lpPos->flags & SWP_FRAMECHANGED)
    {
        // 1. ����ޱ߿�(WS_CAPTION|WS_MAXIMIZEDBOX)������󻯺󸲸�������������
        // 2. ����б߿򴰿���󻯺󣬻ᱻϵͳǿ���޸�Ϊ�̶���С������

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

        // ��󻯣�Ȼ����С������ʱ�ٻ�ԭ������MonitorFromWindow�õ���������Ļ�����������ʱ������Ļ
        //MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);  ��
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

// ����ʾ����ͱ߿�
long  CustomWindow::_OnNcCalcSize(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // chrome: �����������һ���ѡ�������޷�Ӧ
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
        // ��WM_WINDOWPOSCHANGING��ȥ���������ʽ������һ���ӻ�ȥ
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

// �������ʱ��m_nMaximizeBorder�Ĵ���
// Ĭ�ϲ���ϵͳ�ڴ������ʱ��ֻ���������Ϊ��Ļ��С+�߿��С��
// �����һ���Զ��崰�ڵ�m_nMaximizeBorder�ܴ�(��100�������߿��С��ʱ����󻯺󴰿ڵ�maxֵ�Ͳ���ȷ�ˡ�
// ��������������������д�����������������Ĵ������ֵΪ�����Ļ���ܿ�ȣ�
// һ��ûʲô���⣬ֻ��Ҫ���������ɣ�
long CustomWindow::_OnGetMinMaxInfo( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;  // ��windowbase��������

    if (NDEF == m_nMaxWidth || NDEF == m_nMaxHeight)
    {
        MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
        int  nMonitorCount = GetSystemMetrics(SM_CMONITORS);
        if (1 == nMonitorCount)
        {
            // ��Щ����Ҫȫ����������ΧӦ����������Ļ�������ǹ�������
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
//	��ȡһ��POINT��CustomWindow�����λ��
//
int CustomWindow::OnHitTest(POINT* pt, POINT*  ptInChild)
{
	if (m_lResizeCapability == WRSB_NONE)
	{
		return HTCLIENT;
	}
 	else if (m_lResizeCapability == WRSB_CAPTION)
	{
		// TBD: panel ������hittest
		Object* pObjPress = GetPressObject();
		if (IsPanel(pObjPress))
			pObjPress = nullptr;
		
		if (nullptr == pObjPress && !IsZoomed(m_hWnd))
        {
			// ���ʱ��hover obj��û�и���
            Object* pObj = GetObjectByPos(this, pt);

			// TBD: panel ������hittest
			if (IsPanel(pObj))
				pObj = nullptr;

            if (!pObj)
			    return HTCAPTION;
        }
	}
	else if (m_lResizeCapability == WRSB_TRANSPARENT)
	{
		// TBD: panel ������hittest
		Object* pObjPress = GetPressObject();
		if (IsPanel(pObjPress))
			pObjPress = nullptr;

		if (nullptr == pObjPress)
		{
			// ���ʱ��hover obj��û�и���
			Object* pObj = GetObjectByPos(this, pt);

			// TBD: panel ������hittest
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
        // Win7�������ʱ��������ק��ԭ����
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
//	ʹ��ͨ���� NCHitTest�з���HTCAPTIONҲ��ʹ�õ����ק��Ч�����������һ��WM_MOUSELEAVE��Ϣ����Ϊ��겻�ڿͻ�������
//
//	ʹ��WS_THICKFRAMEҲ����ֱ��ʵ�ֱ߿���קЧ����������Ҫȥ��ӦNCCACLSIZE�����һ�����ԣ����ܻᵼ�´�����WIN7������ʾ͸���߿�
//	
//  ����UpdateLayeredWindow֧�ָ�ƽ���Ĵ�������Ч������˶��ڷֲ㴰�ڵ�����������һ���߼�ʵ�ֵ�
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

// TODO: (û������ˣ��´��ٿ���
// ���ڽ��û�б������Ĵ��ڣ���С���ͻ�ԭ��ʱ��û�ж��������⡣
// û�ж���Ҳ�ᵼ�´��ڻ�ԭʱ�����˸(����ʹ����aeroЧ��ʱ)
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

// ע����Ҫ��ӦUIMSG��WM_SIZE����Ϊ��WindowBase::_OnSize�о��Ѿ���ʼ���´����ˣ���
//     ����Ҫ����֮ǰ��m_bNeedToSetWindowRgn��־���ϡ����򽫹����ϱ�־��ȴ�����OnEndEraseBknd
void  CustomWindow::virtualOnSize(unsigned int nType, unsigned int nWidth, unsigned int nHeight)
{
    __super::virtualOnSize(nType, nWidth, nHeight);

	// ��С���ʹ���С����ԭʱ������Ҫ���¼��㴰�ڵ���״
	if (SIZE_MINIMIZED == nType || (SIZE_RESTORED==nType && ::IsIconic(m_hWnd)))
		return;

	this->m_bNeedToSetWindowRgn = true;
}

//
//	���ô����Ƿ������ק
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
// �жϴ��ڵ�����״̬λ
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

    // �����εĸ�ֵ������ֵһ�£��������
    if (pOldMode == pMode)
        pOldMode = nullptr;


    // �����Ǵ�������ʱ�������л����õģ���ʱ��û�д�������
    // �ڴ��ڳ�ʼʱ�ٵ���һ��
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
        pOldMode->Enable(false);  // ע�����ڷֲ㴰�ڵ�disable����Ҫ���õ�ǰ��trans mode���ú�����rgn�󣬲���ȡ��WS_EX_LAYERED��ʽ������ᵼ�´��ڱ��
        SAFE_RELEASE(pOldMode);
    }

    //    m_pRenderChain->GetWindowLayer()->ReCreateRenderTarget();

    // ÿ���Ƿ���Ҫ��ջ��棬����alpha����
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
//	����/ȡ��һ������Ϊ�ֲ㴰��
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

// WS_CAPTION|WS_MAXIMIZEBOX�Ĵ�����󻯺󣬲Ų�����ס������
bool  CustomWindow::IsBorderlessWindow()
{
	LONG_PTR lStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
    if (lStyle & WS_CAPTION)
        return false;

    return true;
}