#include "stdafx.h"
#include "windowbase.h"
#include <Uxtheme.h>

#include "include/interface/ixmlwrap.h"
#include "include/interface/iuires.h"
#include "include/interface/imapattr.h"
#include "src/Atl\image.h"
#include "src/layout/layout.h"
#include "src/Helper\topwindow\topwindowmanager.h"
#include "src/resource/res_bundle.h"
#include "src/Util\dwm\dwmhelper.h"
#include "src/Base\Message\message.h"
#include "src/attribute/attribute.h"
#include "src/attribute/long_attribute.h"
#include "src/attribute/string_attribute.h"
#include "src/application/uiapplication.h"
#include "src/Util\DPI\dpihelper.h"
#include "src/Layer\compositor.h"
#include "include/interface/renderlibrary.h"
#include "include/interface/iuiapplication.h"
#include "include/interface/iuiautotest.h"

using namespace ui;

#ifdef _DEBUG
//  避免有未正确释放的窗口对象，造成内存泄露
class WindowBaseDebug
{
public:
	WindowBaseDebug() { m_nCount = 0; }
	~WindowBaseDebug() { 
		UIASSERT(0 == m_nCount); 
	}
	static void Inc() { m_nCount++; }
	static void Dec() {	m_nCount--;	}
private:
	static int  m_nCount;
};
int WindowBaseDebug::m_nCount = 0;
WindowBaseDebug  g_WindowBaseDebug;
#endif

WindowBase::WindowBase(IWindowBase* p): Panel(p),
m_oWindowRender(this),
m_oMouseManager(*this)
{
#ifdef _DEBUG
	WindowBaseDebug::Inc();
#endif

    m_pIWindowBase = p;
	this->m_hWnd = nullptr;
	this->m_oldWndProc = nullptr;
	this->m_pDefaultFont = nullptr;

	m_bFirsetEraseBkgnd = true;
	this->m_bDoModal = false;
	this->m_bEndModal = false;
	this->m_lDoModalReturn = 0;

	memset(&m_windowStyle, 0, sizeof(m_windowStyle));

	// 取消Panel基本中的透明属性
	OBJSTYLE s = {0};
	s.default_transparent = 1;
	ModifyObjectStyle(0, &s);

	m_pCallbackProxy = nullptr;
}

WindowBase::~WindowBase()
{
#ifdef _DEBUG
	WindowBaseDebug::Dec();
#endif

	UIASSERT( !IsWindow(m_hWnd) );          // 确保窗口已被销毁
	UIASSERT( nullptr == this->m_oldWndProc ); // 确保已经取消子类化
	m_hWnd = nullptr;

	SAFE_RELEASE(m_pDefaultFont);
	SAFE_DELETE(m_pCallbackProxy);
}


//	[public] bool Attach( HWND hWnd )
//
//	目的：和一个现有的窗口进行关联，对该窗口进行子类化操作
//
//	参数：
//		ID
//			[in]	UI窗口在xml中的ID，用于加载子控件
//
//		hWnd
//			[in]	要进行关联的窗口句柄
//	
//	返回：成功返回true，失败返回false
//
//	备注：调用该函数的情况是：有一个窗口（例如派生自CDialog，而不是一个UI的window类），
//		  但它却想使用我们的UI控件，这个时候需要采用这个
//

//
//	增加一个控件的通知者为自己，即当这个控件有事件发生时，将事件通知我
//
//	Parameter
//		idPath
//			[in]	要设置的控件ID，如window/panel/loginbtn，那么idPath应为 panel/loginbtn
//		nNotifyMapID
//			[in]	通知事件的处理ID，即ProcessMessage的第二个参数
//
// void WindowBase::Control_NotifyMe( const String&  idPath, int nNotifyMapID )
// {
// 	this->Control_NotifyMe( __super::GetChildObject( idPath ), nNotifyMapID );
// }
void WindowBase::Control_NotifyMe( Object*  pObj, int nNotifyMapID )
{
	if( nullptr == pObj )
	{
		UI_LOG_ERROR( _T("Window::Control_NotifyMe, 无效的参数 pObj ") );
		return ;
	}

	pObj->SetNotify(this->GetIMessage(), nNotifyMapID);
}

WindowBase::operator HWND() const
{
	return this->m_hWnd;
}

HWND  WindowBase::GetHWND() 
{
    return m_hWnd; 
}

// void  WindowBase::PaintWindow(HDC hDC, RECT* prcInvalidArray, uint nCount)
// {
// #ifdef TRACE_DRAW_PROCESS
// 	if (prcInvalidArray)
// 	{
// 		ATL::CString strOutput(L"---------- InvalidRect:");
// 		for (uint i = 0; i < nCount; i++)
// 		{
// 			strOutput.AppendFormat(
// 				TEXT("%d,%d [%d,%d]  "),
// 				prcInvalidArray[i].left,
// 				prcInvalidArray[i].top, 
// 				RECTW(prcInvalidArray[i]),
// 				RECTH(prcInvalidArray[i]));
// 		}
// 		strOutput.Append(TEXT(" ----------"));
// 		UI_LOG_DEBUG(strOutput);
// 	}
// 	else
// 	{
// 		UI_LOG_DEBUG(_T("InvalidRect: nullptr-----------------------"));
// 	}   
// #endif
// }

void  WindowBase::DestroyWindow()
{
	if (m_hWnd)
	{
		BOOL bRet = ::DestroyWindow(m_hWnd);
		UIASSERT(bRet); // 检查该窗口是否是该线程创建的

        set_hwnd(nullptr);
	}
	m_strId.clear();
}

bool WindowBase::Create(const wchar_t* szId, HWND hWndParent, RECT* prc, long lStyle, long lExStyle)
{
	if (false == this->CreateUI(szId))
	{
		UI_LOG_ERROR(_T("CreateUI failed. id=%s"), szId);
		return false;
	}

	//	创建窗口句柄
	CREATESTRUCT cs;
	::ZeroMemory(&cs, sizeof(CREATESTRUCT));

    cs.hwndParent = hWndParent;
    if (lStyle)
        cs.style = lStyle;
    else
	    cs.style = WS_OVERLAPPEDWINDOW|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN;

    if (lExStyle)
        cs.dwExStyle = lExStyle;
    else
	    cs.dwExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;

	cs.lpszClass = WND_CLASS_NAME;
	cs.lpszName  = _T("");
    if (prc)
    {
        cs.x = prc->left;
        cs.y = prc->top;
        cs.cx = prc->right - prc->left;
        cs.cy = prc->bottom - prc->top;
        m_windowStyle.setcreaterect = 1;
    }
    else
    {
	    cs.x = cs.y  = 0;
	    cs.cx = cs.cy = CW_USEDEFAULT;  //500; 这里不能直接写一个值。有可能窗口配置的也是这个大小，将导致收不到WM_SIZE消息，布局失败
    }

    UISendMessage(this, UI_MSG_PRECREATEWINDOW, (WPARAM)&cs);

	s_create_wnd_data.AddCreateWndData(&m_thunk.cd, this);
	this->m_hWnd = ::CreateWindowEx( 
		cs.dwExStyle,
		cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy,
		cs.hwndParent, 0, nullptr, nullptr );

	if (nullptr == m_hWnd)
	{
		UI_LOG_FATAL( _T("CreateWindowEx失败"));
		return false;
	}
	return true;
}

bool WindowBase::Attach(HWND hWnd, const wchar_t* szID)
{
	if (m_hWnd)
		return false;

    set_hwnd(hWnd);

	if (false == this->CreateUI(szID))
	{
        set_hwnd(nullptr);
		UI_LOG_ERROR(_T("failed. id=%s"), m_strId.c_str());
		return false;
	}

	m_windowStyle.attach = 1;
	this->m_thunk.Init( &WindowBase::ThunkWndProc, this );
	WNDPROC pProc = this->m_thunk.GetWNDPROC();

	this->m_oldWndProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);

	BOOL bHandled = FALSE;
	this->_OnCreate(0,0,0,bHandled);

	return true;
}

void  WindowBase::Detach()
{
	if (nullptr == m_hWnd)
		return;

	BOOL bHandled = FALSE;
	_OnNcDestroy(WM_NCDESTROY, 0, 0, bHandled);
    set_hwnd(nullptr);
}

// EndDialog并不会立即销毁模态窗口，而是加一个标志位，先结束消息循环，
// 然后再销毁窗口
void WindowBase::EndDialog(INT_PTR nResult)
{
	UIASSERT( this->m_bDoModal );

	// 让ModalWindow::DoModal里的消息循环能够退出
	this->m_lDoModalReturn = (long)nResult ;
	this->m_bEndModal = true;
	::PostMessage( this->m_hWnd, WM_NULL, 0,0 );
}

INT_PTR WindowBase::DoModal(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szID, HWND hWndParent )
{
#if 0
	UIASSERT( nullptr == m_hWnd );

	UI_AddCreateWndData(&m_thunk.cd, (void*)this);
	m_strID = ID;

	return DialogBox(hResInst, MAKEINTRESOURCE(nResID), hWndParent, (DLGPROC)WindowBase::StartDialogProc);
#endif


	HWND hWnd = this->DoModeless(hResInst, nResID, szID, hWndParent);
	if( nullptr == hWnd )
	{
		return -1;
	}

	return this->ModalLoop(hWndParent);
}


//
// 创建一个空的模态对话框
//
INT_PTR WindowBase::DoModal(const wchar_t* szID, HWND hWndParent, bool canResize)
{

#if 0
	UIASSERT( nullptr == m_hWnd );

	CREATESTRUCT cs;
	cs.style        = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
	cs.lpszClass    = WND_CLASS_NAME;
	cs.lpszName     = _T("");
	cs.x  = cs.y    = 0;
	cs.cx = cs.cy   = 100;//CW_USEDEFAULT;
	DWORD dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;
	this->PreCreateWindow( cs );

	HGLOBAL hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
	if (!hgbl)
		return -1;
	LPDLGTEMPLATE lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	lpdt->cdit = 0;  // Number of controls
	lpdt->style = cs.style;
	lpdt->dwExtendedStyle = dwStyleEx;
	lpdt->x = cs.x;
	lpdt->y = cs.y;
	lpdt->cx = cs.cx;
	lpdt->cy = cs.cy;

	LPWORD lpw = (LPWORD)(lpdt + 1);
	*lpw++ = 0;             // No menu
	*lpw++ = 0;             // Predefined dialog box class (by default)

	LPWSTR lpwsz = (LPWSTR)lpw;
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // 窗口的标题名称
	lpw += nchar;
	GlobalUnlock(hgbl); 

	UI_AddCreateWndData(&m_thunk.cd, this);
	m_strID = ID;

	long lRet = DialogBoxIndirect(/*m_pUIApplication->GetModuleInstance()*/g_hInstance, 
		(LPDLGTEMPLATE)hgbl, 
		hWndParent, 
		(DLGPROC)WindowBase::StartDialogProc); 

	GlobalFree(hgbl); 
	return lRet;
#endif
// 	HWND hWnd = this->DoModeless(szID, hWndParent, canResize);
// 	if( nullptr == hWnd )
// 	{
// 		return -1;
// 	}

	Create(szID, hWndParent, nullptr);
	return this->ModalLoop(hWndParent);
}

INT_PTR WindowBase::ModalLoop(HWND hWndParent)
{
    // 给外部一个设置模态窗口显示的机会
//     UIMSG  msg;
//     msg.message = UI_WM_SHOWMODALWINDOW;
//     msg.pMsgFrom = msg.pMsgTo = m_pIWindowBase;
//     long lRet = UISendMessage(&msg);
// 
//     if(msg.bHandled && lRet)
//     {
// 
//     }
//     else
    {
    	this->ShowWindow();
	    ::UpdateWindow(this->m_hWnd);
    }
	
	this->m_bDoModal = true;
	bool bEnableWindow = false;
	if (hWndParent && GetDesktopWindow() != hWndParent && IsWindowEnabled(hWndParent))
	{
		::EnableWindow( hWndParent, FALSE );
		bEnableWindow = true;
	}

#if 0
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (FALSE == m_pUIApplication->IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (this->m_bEndModal)
		{
			this->m_bEndModal = false;
			break;
		}
	}
#else
	GetUIApplication()->MsgHandleLoop(&m_bEndModal);
	m_bEndModal = false;  // 还原，用于下次DoModal

#endif
	// 窗口的销毁要放在这里，不能提前销毁。必须在销毁之前设置父界面为active，否则
	// 会造成窗口闪烁
	UIASSERT(IsWindow(m_hWnd));

	// hide the window before enabling the parent, etc.
	BOOL bActive = ::GetActiveWindow() == m_hWnd;

	if (bEnableWindow)
	{
		::EnableWindow( hWndParent, TRUE );
	}
	if (hWndParent && bActive)
	{
		// 如果不调用该函数，将导致父窗口跑到屏幕Z次序的后面去了
		::SetActiveWindow(hWndParent); 
	}
	if (m_hWnd != nullptr)
	{
		::ShowWindow(m_hWnd, SW_HIDE);
	}

	this->m_bDoModal = false;
	::DestroyWindow(this->m_hWnd);    // 销毁窗口
	
	return this->m_lDoModalReturn;
}



// 2014.4.15
// 注：在win7 aero下面，无WS_THICKFRAME样式的Dialog会变大10px,10px
//     为了解决该问题，为每一个Dialog加上WS_THICKFRAME，同时修改WM_NCHITTEST消息、删除SysMenu中的SC_SIZE.
///    因此外部在调用该函数时，需要传递一个canResize标志
//
HWND WindowBase::DoModeless(const wchar_t* szId, HWND hWndOnwer, bool canResize)
{
	UIASSERT(nullptr == m_hWnd);

	if (false == this->CreateUI(szId))
	{
		UI_LOG_ERROR(_T("CreateUI failed. id=%s"), szId);
		return false;
	}

	CREATESTRUCT cs;
	memset(&cs, 0, sizeof(CREATESTRUCT));
	cs.style        = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION | WS_THICKFRAME;
	cs.lpszClass    = WND_CLASS_NAME;
	cs.lpszName     = _T("");
	cs.x  = cs.y    = 0;
	cs.cx = cs.cy   = 0;
	cs.dwExStyle    = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;
    UISendMessage(this, UI_MSG_PRECREATEWINDOW, (WPARAM)&cs);

	HGLOBAL hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
	if (!hgbl)
		return nullptr;
	LPDLGTEMPLATE lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	lpdt->cdit = 0;  // Number of controls
	lpdt->style = cs.style;
	lpdt->dwExtendedStyle = cs.dwExStyle;
	lpdt->x = (short)cs.x;
	lpdt->y = (short)cs.y;
	lpdt->cx = (short)cs.cx;
	lpdt->cy = (short)cs.cy;

	LPWORD lpw = (LPWORD)(lpdt + 1);
	*lpw++ = 0;             // No menu
	*lpw++ = 0;             // Predefined dialog box class (by default)

	LPWSTR lpwsz = (LPWSTR)lpw;
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // 窗口的标题名称
	lpw += nchar;
	GlobalUnlock(hgbl); 

	s_create_wnd_data.AddCreateWndData(&m_thunk.cd, this);
	{
		m_hWnd = CreateDialogIndirect(
					/*Application::GetModuleInstance()*/g_hInstance, 
					(LPDLGTEMPLATE)hgbl, 
					hWndOnwer, 
					(DLGPROC)WindowBase::StartDialogProc); 
	}
	if (m_hWnd)
	{
		// fix默认的#32770窗口过程在大小改变时不刷新的问题
		// TODO:?? 为什么带THICKFRAME的窗口就能自己刷新
		SetClassLongPtr(m_hWnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS);

        // 删除最大化
        if (!canResize)
        {
            HMENU hMenu = GetSystemMenu(m_hWnd, FALSE);
            if (hMenu)
            {
                DeleteMenu(hMenu, SC_SIZE, 0);
            }
			m_windowStyle.dialog_noresize = 1;
        }
	}
	GlobalFree(hgbl); 
	return m_hWnd;
}

HWND WindowBase::DoModeless(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szId, HWND hWndOnwer)
{
	UIASSERT( nullptr == m_hWnd );

	if (false == this->CreateUI(szId))
	{
		UI_LOG_ERROR(_T("CreateUI failed. id=%s"), szId);
		return false;
	}

	s_create_wnd_data.AddCreateWndData(&m_thunk.cd, (void*)this);

    // 注： hWndParent仅是Owner，并不是parent
	m_hWnd = CreateDialog(hResInst, MAKEINTRESOURCE(nResID), hWndOnwer, (DLGPROC)WindowBase::StartDialogProc);

	LONG_PTR lStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
    if (!(lStyle & WS_CHILD))
    {
        if (! (lStyle&WS_THICKFRAME))
        {
            CRect  rcWindow;
            ::GetWindowRect(m_hWnd, &rcWindow);

            SetWindowLongPtr(m_hWnd, GWL_STYLE, lStyle|WS_THICKFRAME);

            HMENU hMenu = GetSystemMenu(m_hWnd, FALSE);
            if (hMenu)
            {
               DeleteMenu(hMenu, SC_SIZE, 0);
            }
			m_windowStyle.dialog_noresize = 1;

            // 重新修改窗口为正确的大小
            SetWindowPos(m_hWnd, 0, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
        }
    }
	return m_hWnd;
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//
//	[static] Dialog类型窗口的第一个窗口消息调用的窗口过程
//
long CALLBACK WindowBase::StartDialogProc( HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam )
{
	// 获取this指针
	WindowBase* pThis = (WindowBase*)s_create_wnd_data.ExtractCreateWndData();
	UIASSERT(nullptr != pThis);

	if( nullptr == pThis )
		return 0;

	return pThis->StartProc(hwnd,uMsg,wParam,lParam, false);
//	return (UINT_PTR)FALSE;
}

long WindowBase::DefWindowProc( unsigned int uMsg, WPARAM wParam, LPARAM lParam )
{
	if (m_oldWndProc)
	{
		return ::CallWindowProc( m_oldWndProc, m_hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

long WindowBase::_OnSetCursor( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if (0 == HIWORD(lParam))  // is menu popup
	{ 
		::SetCursor( ::LoadCursor(nullptr, IDC_ARROW) );
		return TRUE;
	}

	HWND hWnd = (HWND)wParam;
	if (0 != wParam && hWnd != m_hWnd)
	{
		return DefWindowProc(uMsg,wParam,lParam);  // 鼠标位于窗口上的其它windows控件上面，不处理
	}  
	else if (hWnd == m_hWnd)  
	{
		// TODO: 这里需要区分window/dialog吗
        unsigned int nHitTest = LOWORD(lParam);
        if (m_oldWndProc)
        {
            if (nHitTest != HTCLIENT)   
            {
                SetCursorByHitTest(nHitTest);
                return TRUE;
            }
        }
        else
        {
            // 用于鼠标位于system window的边缘时，应该调用默认的处理过程。
            // 包括当自己弹出一个模态框，返回HTERROR时
		    if (nHitTest != HTCLIENT)    
		    {
			    //return DefWindowProc(uMsg,wParam,lParam);  // 2014.4.10 dialog直接return 0居然没反应，不会根据hittest设置鼠标。因此直接调用defwindowproc
                return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		    }
        }

//      Removed 20120824 -- 在实现windowless richedit时，不能采用延时发送setcursor的办法，否则在
//      选择文本时鼠标样式一直闪烁。因此修改方案为：先发送，然后在WM_MOUSEMOVE中检测如果hover obj
//      发生了变化的话，重新再发送一次WM_SETCURSOR
	}
	if (TRUE == m_oMouseManager.HandleMessage(uMsg, wParam, lParam, &bHandled)) // 仅发给当前hover object
	{
		return TRUE;
	}

	// 让窗口去处理这个消息
	UIMSG msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam  = wParam;
	msg.lParam  = lParam;
	if ((this->GetIMessage())->ProcessMessage(&msg, 0)) 
		return msg.lRet;

	// 还是没有人处理
	::SetCursor(::LoadCursor(nullptr,IDC_ARROW));
	return TRUE;
}

void  WindowBase::SetCursorByHitTest(unsigned int nHitTest)
{
    static HCURSOR _arrow = ::LoadCursor(nullptr, IDC_ARROW);

    switch(nHitTest)
    {
    case HTTOPLEFT:
    case HTBOTTOMRIGHT:
        {
            static HCURSOR _cursor = ::LoadCursor(nullptr, IDC_SIZENWSE);
            ::SetCursor(_cursor);
        }
        break;

    case HTTOP:
    case HTBOTTOM:
        {
            static HCURSOR _cursor = ::LoadCursor(nullptr, IDC_SIZENS);
            ::SetCursor(_cursor);
        }
        break;

    case HTLEFT:
    case HTRIGHT:
        {
            static HCURSOR _cursor = ::LoadCursor(nullptr, IDC_SIZEWE);
            ::SetCursor(_cursor);
        }
        break;

    case HTTOPRIGHT:
    case HTBOTTOMLEFT:
        {
            static HCURSOR _cursor = ::LoadCursor(nullptr, IDC_SIZENESW);
            ::SetCursor(_cursor);
        }
        break;

    case HTCAPTION:
        ::SetCursor(_arrow);
        break;

    default:
        ::SetCursor(_arrow);
        break;	
    }
}

long  WindowBase::_OnNcHitTest( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;
    if (!m_oldWndProc)
    {
        if (m_windowStyle.dialog_noresize)
        {
            long lr = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
            if (lr >= HTLEFT && lr <= HTBORDER)  // 用HTTOPRIGHT还有是问题，非得用HTBORDER，为什么
            {
                lr = HTBORDER;
                bHandled = TRUE;
                return lr;
            }
        }
    }
    return 0;
}


long WindowBase::_OnCreate(
        unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

    m_oWindowRender.BindHWND(m_hWnd);

    // 特殊处理：
    // 用于将D2D RenderTarget绑定到窗口上面.D2D与GDI/GDIPlus
    // 不一样，它不需要再弄一个双缓冲，直接begindraw/enddraw即可
// 	if (m_pRenderLayer)
// 	{
// 		IRenderTarget*  pRT = m_pRenderLayer->GetRenderTarget();
// 		if (pRT)
// 		{
// 			pRT->BindHWND(m_hWnd);
// 		}
// 	}

	//
	//  有可能m_strID为空（不加载资源，例如临时的popupconotrolwindow）
	//	因此没有将AddTopWindowObject、OnInitWindow放在CreateUI中执行
	//
	// if (!IsChildWindow())--子窗口也是一个UI窗口，也维护起来
	{
		TopWindowManager* pTopWndMgr = 
            GetUIApplication()->GetTopWindowMgr();
		if (pTopWndMgr)
			pTopWndMgr->AddTopWindowObject(this);
	}

	// 布局
	if (m_windowStyle.attach)  // attach的窗口直接使用外部的大小
	{
		::GetClientRect(m_hWnd, &m_rcParent);

        // 避免此时调用GetDesiredSize又去测量窗口大小了，
        // 导致窗口被修改为自适应大小 
		CRect rcWindow;               
		::GetWindowRect(m_hWnd, &rcWindow);
		SetConfigWidth(rcWindow.Width());
		SetConfigHeight(rcWindow.Height());

        // 因为Attach到的窗口初始化时已经收不到WM_SIZE了，
        // 因此自己再发一次，
        // 通知创建RenderTarget，否则后面的一些刷新将失败
		notify_WM_SIZE(0, m_rcParent.Width(), m_rcParent.Height());
        this->UpdateLayout();
	}
    else
    {
        if (m_windowStyle.setcreaterect)
        {
            // 避免此时调用GetDesiredSize又去测量窗口大小了，
            // 导致窗口被修改为自适应大小 
            CRect rcWindow;    
            ::GetWindowRect(m_hWnd, &rcWindow);
            SetConfigWidth(rcWindow.Width());
            SetConfigHeight(rcWindow.Height());

            ::GetClientRect(m_hWnd, &m_rcParent);
            this->UpdateLayout();
        }
        else
        {
            // 不能放在 OnInitialize 后面。
            // 因为有可能OnInitialize中已经调用过 SetWindowPos
            DesktopLayout dl;  
            dl.Arrange(this);
        }
    }

    if (!m_strConfigWindowText.empty())
        ::SetWindowText(m_hWnd, m_strConfigWindowText.c_str());

    // 创建默认字体
    if (!m_pDefaultFont)
        SetDefaultRenderFont(L"");


    // 防止在实现显示动画时，先显示了一些初始化中刷新的内容。
    // 注：不能只限制一个layer
    m_oWindowRender.SetCanCommit(false); 
    {
		// 给子类一个初始化的机会 (virtual)，
		// 例如设置最大化/还原按钮的状态
		this->virtualInnerInitWindow();

		m_objStyle.initialized = 1;
        UISendMessage(m_pIMessage, UI_MSG_INITIALIZE);
        ForwardInitializeMessageToDecendant(this);
		UISendMessage(m_pIMessage, UI_MSG_INITIALIZE2);
    }
	if (m_pCallbackProxy)
	{
		m_pCallbackProxy->DoBindPlz(true);
	}
    if (m_pCallbackProxy)
    {
        m_pCallbackProxy->OnWindowInit();
    }
	

	m_oWindowRender.SetCanCommit(true);

	// 设置默认对象
	m_oMouseManager.SetDefaultObject(
            m_oMouseManager.GetOriginDefaultObject(),
            false);

    if (m_windowStyle.attach) // 主动触发刷新
    {
        Invalidate();
    }

	IUIAutoTest* pAutoTest = GetUIApplication()->GetUIAutoTestPtr();
	if (pAutoTest)
	{
		pAutoTest->OnWindowInit(
			static_cast<IWindow*>(m_pIWindowBase));
	}

	return 0;
}

void  WindowBase::virtualInnerInitWindow()
{
}

long  WindowBase::_OnClose(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;  // 默认处理会调用DestroyWindow

    // 先发送给订阅者，例如cef控件，它需要异步关闭
    bool bCanClose = true;
    on_close.emit_foreach(
        [&bCanClose]() -> bool
    {
        // 有一个订阅者中断关闭
        if (!bCanClose)
            return false;
        return true;
    },
        bCanClose);
    if (!bCanClose)
    {
        bHandled = TRUE;
        return 0;
    }

    if (m_pCallbackProxy)
    {
        bool bCanClose = true;
        m_pCallbackProxy->OnWindowClose(bCanClose);
        if (!bCanClose)
        {
            bHandled = TRUE;
            return 0;
        }
    }
	
	if (m_bDoModal)
	{
		EndDialog(IDCANCEL);
		bHandled = TRUE;
		return 0;
	}
	return 0;
}

long WindowBase::_OnHandleMouseMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_syncWindow.IsSizeMoveIng())  // 拖拽过程中不处理
	{
		bHandled = FALSE;
		return 0;
	}

	bHandled = FALSE;
	long lRet = m_oMouseManager.HandleMouseMessage(uMsg, wParam, lParam, &bHandled);

    if (bHandled)
        return lRet;

// 	if (m_oMouseManager.GetHoverObject() || m_oMouseManager.GetPressObject())
// 	{
// 		bHandled = TRUE;
// 	}
	return 0;
}
long  WindowBase::_OnHandleKeyBoardMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    return m_oMouseManager.HandleKeyboardMessage( uMsg, wParam, lParam, &bHandled );
}

long  WindowBase::_OnHandleTouchMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    return m_oMouseManager.HandleTouchMessage( uMsg, wParam, lParam, bHandled);
}


long WindowBase::_OnSetFocus( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;


	m_oMouseManager.HandleMessage( uMsg, wParam, lParam, &bHandled);

	UIMSG msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam  = wParam;
	msg.lParam  = lParam;
    UISendMessage(&msg);

	return 1;    // 注：由于Dialog默认的WM_SETFOCUS会将焦点再设置到自己的第一个控件上面，因此如果在这里return 0或者bHandled = FALSE
	             //     将导致::SetFocus(m_hWnd)的焦点到窗口上面后，又被窗口自己设置到控件上面，导致SetFocus达不到我们的本意
}
long WindowBase::_OnKillFocus( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	m_oMouseManager.HandleMessage( uMsg, wParam, lParam, &bHandled);

	UIMSG msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam = wParam;
	msg.lParam = lParam;
//	this->ProcessMessage( &msg );
    UISendMessage(&msg);

	return 1 ;
}

// 用于解决如果WindowBase是一个子窗口，放在一个普通顶层窗口上，那么点击WindowBase
// 将无法得到焦点。因此在这里响应MouseActive消息，将自己SetFocus
// 至少像系统按钮控件是如何处理的点击得到焦点就不得而知了，但WS_TABSTOP不是原因，试过了，没用。
long WindowBase::_OnMouseActive( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;
    if (IsChildWindow() && ::GetFocus() != m_hWnd)
    {
        ::SetFocus(m_hWnd);
    }
    return 0;
}

long WindowBase::_OnThemeChange( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	
    UIMSG  msg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;

	Object::ForwardMessageToChildObject(this, &msg);
	return 0;
}

HMONITOR  WindowBase::GetPrimaryMonitor()
{
    POINT pt = {0,0};
    return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY); 
}


// minmax.c xxxInitSendValidateMinMaxInfo
//
// 1. 如果最大化到副屏上时，ptMaxSize, ptMaxPosition指的是主屏的大小。
//    For systems with multiple monitors, the ptMaxSize and ptMaxPosition members
//    describe the maximized size and position of the window on the primary monitor, 
//    even if the window ultimately maximizes onto a secondary monitor. 

// 2. 系统内部会判断如果pinfo的maxsize大于主屏宽高时，将相应的调整为副屏的大小。
//    In that case, the window manager adjusts these values to compensate for differences
//    between the primary monitor and the monitor that displays the window. Thus, if 
//    the user leaves ptMaxSize untouched, a window on a monitor larger than the primary
//    monitor maximizes to the size of the larger monitor.
//
// 3. 但是!!!对于一个窗口 -- 与是否有边框无关。
//
//     . 有边框的窗口会多出若干像素border，它的ptMaxSize本身就比主屏分辨率大6~8px,不容易出现这个问题。
//     . 无边框的窗口最大化会出现覆盖任务栏变成全屏的问题，因此需要对ptMaxSize作缩进，很容易出现这种问题
//       (测试发现，只有同时具备了WS_CAPTION|WS_MAXIMIZEBOX属性的窗口最大化后，才不会覆盖任务栏）
//  
//    如果我们给出的ptMaxSize（与ptMaxPosition无关）
//    小于主屏分辨率大小时，最大化到副屏的窗口不会自适应,仍然是参数中的ptMaxSize。
//    如果副屏更大的话，就造成最大化不完全的问题。
//
// 4. ptMaxPosition，Windows内部是基于主屏的workarea。当任务栏位于左侧时,ptMaxPostion.x=1会自动调整会
//    任务栏的右侧1px处
//
// 5. ptMaxSize，Windows内部是与主屏的monitorarea比较的。当ptMaxSize大小主屏大小时，Windows会自动
//    将窗口调整为workarea大小。但如果ptMaxSize小于主屏大小时，Windows不做调整
//
long WindowBase::_OnGetMinMaxInfo( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    // 对于WindowBase，只对窗口最小值和最大值对处理。
    // 其它逻辑由CustomWindow去处理

	bHandled = TRUE;
	MINMAXINFO* pInfo = (MINMAXINFO*)lParam;

	if (NDEF != m_nMaxWidth)
		pInfo->ptMaxSize.x = pInfo->ptMaxTrackSize.x = m_nMaxWidth;

	if (NDEF != m_nMaxHeight)
		pInfo->ptMaxSize.y = pInfo->ptMaxTrackSize.y = m_nMaxHeight;

	if (NDEF != m_lMinWidth)
		pInfo->ptMinTrackSize.x = m_lMinWidth;

	if (NDEF != m_lMinHeight)
		pInfo->ptMinTrackSize.y = m_lMinHeight;

	return 0;
}

long WindowBase::_OnWindowPosChanging( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    m_syncWindow._OnWindowPosChanging((LPWINDOWPOS)lParam, bHandled);
	return 0;
}

long  WindowBase::_OnShowWindow(
            unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    m_syncWindow._OnShowWindow(wParam, lParam);
    return 0;
}
long WindowBase::_OnSyncWindow( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	switch(wParam)
	{
	case ADD_SYNC_WINDOW:
		{
			const SyncWindowData& data = *(SyncWindowData*)(lParam);
			m_syncWindow.AddAnchorItem(data);
		}
		break;
	case MODIFY_SYNC_WINDOW:
		{
			const SyncWindowData& data = *(SyncWindowData*)(lParam);
            m_syncWindow.ModifyAnchorItem(data);
		}
		break;
	case REMOVE_SYNC_WINDOW:
		{
			const SyncWindowData& data = *(SyncWindowData*)(lParam);
            m_syncWindow.RemoveAnchorItem(data.m_hWnd);
		}
		break;
	case ADD_SYNC_WINDOW_RESULT:
		{
            m_syncWindow.OnAddAnchorItem((HWND)lParam);
		}
		break;
	case MODIFY_SYNC_WINDOW_RESULT:
		{
            m_syncWindow.OnModifyAnchorItem((HWND)lParam);
		}
		break;
	case REMOVE_SYNC_WINDOW_RESULT:
		{
            m_syncWindow.OnRemoveAnchorItem((HWND)lParam);
		}
		break;
	case HOST_WINDOW_DESTROYED:
		{
            m_syncWindow.OnRemoveAnchorItem((HWND)lParam);
		}
		break;
	case HOST_WINDOW_POSCHANGING:
		{
            m_syncWindow.OnHostWindowPosChanging();
		}
		break;

    case SYNC_NOW:
    {
        m_syncWindow.SyncNow();
    }
    break;

	}
	return 0;
}
long WindowBase::_OnEnterSizeMove( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
    m_syncWindow._OnEnterSizeMove();

	long lRet = m_oMouseManager.HandleMouseMessage(uMsg, wParam, lParam, &bHandled);
	if (bHandled)
		return lRet;

	return 0;
}
long WindowBase::_OnExitSizeMove( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
    m_syncWindow._OnExitSizeMove();

	long lRet = m_oMouseManager.HandleMouseMessage(uMsg, wParam, lParam, &bHandled);
	if (bHandled)
		return lRet;
	return 0;
}	

long  WindowBase::_OnGetObject( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (!m_hWnd)
		return 0;

	IAccessible *pAccessible = nullptr;
	this->CreateAccesible(&pAccessible);
	if (pAccessible)
	{
		long lres = LresultFromObject(IID_IAccessible, wParam, pAccessible);
		pAccessible->Release();
		return lres;
	}
	return 0;
}

IMKMgr* WindowBase::virtualGetIMKMgr()
{
    return static_cast<IMKMgr*>(&m_oMouseManager);
}

#if 0
//
//	将双缓存数据提交到窗口DC上
//
void WindowBase::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount)
{
	IRenderTarget* pRenderTarget = m_pRenderLayer->GetRenderTarget();

	POINT  ptSrc = {0, 0};
    m_pRenderLayer->GetOffsetDrawInBuffer(&ptSrc);

	HDC hDC = hDCWnd;
	if (nullptr == hDC)
    {
        if (!m_hWnd)  // 窗口已销毁了。
            return;

		hDC = GetDC(m_hWnd);
    }
    
    HRGN hRgn = nullptr;
    CRect rcCommit;

    
	if (nullptr == prcCommit)
	{
		::GetClientRect(m_hWnd, &rcCommit);
		nRectCount = 1;
	}
    else if (nRectCount == 1)
    {
        CopyRect(&rcCommit, prcCommit);
        OffsetRect(&rcCommit, ptSrc.x, ptSrc.y);
    }
    // 多个区域时，同时提交，避免闪烁
    else //if (nRectCount > 1)
    {
        RECT rc;
        CopyRect(&rc, prcCommit);
        OffsetRect(&rc, ptSrc.x, ptSrc.y);
        UnionRect(&rcCommit, &rcCommit, &rc);

        hRgn = CreateRectRgnIndirect(&rc);
        for (int i = 1; i < nRectCount; ++i)
        {
            if (::IsRectEmpty(&prcCommit[i]))
                continue;

            CopyRect(&rc, &prcCommit[i]);
            OffsetRect(&rc, ptSrc.x, ptSrc.y);
            UnionRect(&rcCommit, &rcCommit, &rc);

            HRGN hRgn2 = CreateRectRgnIndirect(&rc);
            CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
            DeleteObject(hRgn2);
        }
    }

    if (hRgn)
    {
        SaveDC(hDC);
        ExtSelectClipRgn(hDC, hRgn, RGN_AND);
    }

    Render2TargetParam  param = {0};
    param.xDst = rcCommit.left;
    param.yDst = rcCommit.top;
    param.wDst = rcCommit.Width();
    param.hDst = rcCommit.Height();
    param.xSrc = rcCommit.left;
    param.ySrc = rcCommit.top;
    param.wSrc = rcCommit.Width();
    param.hSrc = rcCommit.Height();
    param.bAlphaBlend = false;
    param.opacity = 255;

    pRenderTarget->Render2DC(hDC, &param);

#ifdef TRACE_DRAW_PROCESS
    UI_LOG_DEBUG(_T("Commit2Wnd: %d,%d [%d,%d]"),
        rcCommit.left, rcCommit.top,
        RECTW(rcCommit), RECTH(rcCommit));
#endif

   if (hRgn)
   {
       RestoreDC(hDC, -1);
       DeleteObject(hRgn);
   }

	if (nullptr == hDCWnd)
	{
		ReleaseDC(m_hWnd, hDC);
	}
}
#endif

void WindowBase::OnGetDesiredSize(SIZE* pSize)
{
    if (nullptr == pSize)
        return;

	Panel::OnGetDesiredSize(pSize);

    // 这里获取到的是client大小，需要转换成window大小
    this->CalcWindowSizeByClientSize(*pSize, pSize);
}

//
//	如果已知一个窗口的client区域大小，求这个窗口的window区域大小
//
//	参数
//		sizeClient
//			[in]	该窗口的客户区大小
//		pSizeWindow
//			[out]	返回窗口的window大小
//
void WindowBase::CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow )
{
	if( nullptr == pSizeWindow )
		return;

	CRect  tempRcWindow, tempRcClient;

	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;

	// 通过 WM_NCCALCSIZE 消息，让系统告诉我们一个window rect 对应的client rect是多少
	np.lppos = &wp;
	::GetWindowRect( this->m_hWnd, &np.rgrc[0] );
	tempRcWindow.CopyRect(&np.rgrc[0]);
	::SendMessage( this->m_hWnd, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&np );
	tempRcClient.CopyRect(&np.rgrc[0] );

	pSizeWindow->cx = sizeClient.cx + tempRcWindow.Width() - tempRcClient.Width();
	pSizeWindow->cy = sizeClient.cy + tempRcWindow.Height() - tempRcClient.Height();
}

//
//	如果已知一个窗口的window区域大小，求这个窗口的client区域大小
//
//	参数
//		rcWindow
//			[in]	该窗口的客大小
//		rcClient
//			[out]	返回窗口的client大小（默认left=0,top=0，即只返回窗口大小）
//
void WindowBase::CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient )
{
	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;

	// 通过 WM_NCCALCSIZE 消息，让系统告诉我们一个window rect 对应的client rect是多少
	np.lppos = &wp;
	::CopyRect( &np.rgrc[0], rcWindow );
	::SendMessage( this->m_hWnd, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&np );

	rcClient->left   = rcClient->top = 0;
	rcClient->right  = np.rgrc[0].right - np.rgrc[0].left;
	rcClient->bottom = np.rgrc[0].bottom - np.rgrc[0].top;
}

// CommitWindowBufferListenerMgr&  WindowBase::GetCommitWindowBufferListenerMgr()
// {
// 	return m_MgrCWBL;
// }
// void  WindowBase::AddCommitWindowBufferListener(ICommitWindowBufferListener* p) 
// { 
//     m_MgrCWBL.AddListener(p); 
// }
// void  WindowBase::RemoveCommitWindowBufferListener(ICommitWindowBufferListener* p) 
// { 
//     m_MgrCWBL.RemoveListener(p); 
// }

Object* WindowBase::GetHoverObject()
{
	return m_oMouseManager.GetHoverObject();
}
Object* WindowBase::GetPressObject()
{
	return m_oMouseManager.GetPressObject();
}
void  WindowBase::SetPressObject(Object* p)
{
	m_oMouseManager.SetHoverObject(p);
	m_oMouseManager.SetPressObject(p);
}

void WindowBase::ShowWindow()
{
	::ShowWindow( m_hWnd, SW_SHOW );
}
void WindowBase::HideWindow()
{
	::ShowWindow( m_hWnd, SW_HIDE );
}

void  WindowBase::CenterWindow(HWND hWndCenter)
{
    // Copy  CWindowImpl::CenterWindow()
    CWindow wnd;
    wnd.Attach(m_hWnd);
    wnd.CenterWindow(hWndCenter);
    wnd.Detach();
}

void  WindowBase::CenterWindow(HMONITOR hMonitor)
{
    CRect  rcDlg;
    CRect  rcArea;

    ::GetWindowRect(m_hWnd, &rcDlg);
    
    if (!hMonitor)
    {
        POINT pt = {0};
        hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }
    if(hMonitor)
    {
        MONITORINFO  mi = {sizeof(mi)};
        GetMonitorInfo(hMonitor, &mi);
        rcArea.CopyRect(&mi.rcWork);
    }
    else
    {
        ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArea, 0);
    }

    int DlgWidth = rcDlg.Width();
    int DlgHeight = rcDlg.Height();

    int xLeft = (rcArea.left + rcArea.right) / 2 - DlgWidth / 2;
    int yTop = (rcArea.top + rcArea.bottom) / 2 - DlgHeight / 2;

    // if the dialog is outside the screen, move it inside
    if(xLeft + DlgWidth > rcArea.right)
        xLeft = rcArea.right - DlgWidth;
    if(xLeft < rcArea.left)
        xLeft = rcArea.left;

    if(yTop + DlgHeight > rcArea.bottom)
        yTop = rcArea.bottom - DlgHeight;
    if(yTop < rcArea.top)
        yTop = rcArea.top;

    ::SetWindowPos(m_hWnd, nullptr, xLeft, yTop, -1, -1,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// 使用桌面布局重新更新窗口位置大小，主要用于在UIEditor中修改了
// 窗口对象的大小。而使用Object::UpdateLayout仅能重新布局子对象的问题
void  WindowBase::UpdateDesktopLayout()
{
    DesktopLayout dl;
    dl.Arrange(this);
}

void WindowBase::SaveMemBitmap(TCHAR* szFile)
{
	UIASSERT(0 && _T("TODO:"));
	return;
#ifdef _DEBUGx
	if (nullptr == m_hMemBitmap)
		return;

	::SelectObject(m_hMemDC, m_hOldBitmap);
	Image image;
	image.Attach(m_hMemBitmap);
//	image.ForceUseAlpha();
	image.Save(szFile, Gdiplus::ImageFormatBMP);
	image.Detach();
	::SelectObject(m_hMemDC, m_hMemBitmap);
#endif
}

// 将内存位图绘图到指定区域
void  WindowBase::AlphaBlendMemBitmap(HDC hDC, RECT* prc, int alpha)
{
	Layer* pLayer = GetLayer();
	if (!pLayer)
		return;

	if (pLayer->GetType() != Layer_Software)
	{
		UIASSERT(0);
		return;
	}

	IRenderTarget* pRT = pLayer->GetRenderTarget();
	HDC hMemDC = pRT->GetHDC();
	POINT  ptOffset = { 0, 0 };

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
	AlphaBlend(hDC, 
		prc->left,
		prc->top, 
		RECTW(prc), 
		RECTH(prc),
		hMemDC, 
		ptOffset.x, 
		ptOffset.y, 
		RECTW(prc), 
		RECTH(prc),
		bf);
}

void  WindowBase::BitBltMemBitmap(HDC hDC, RECT* prc)
{
	Layer* pLayer = GetLayer();
	if (!pLayer)
		return;

	if (pLayer->GetType() != Layer_Software)
	{
		UIASSERT(0);
		return;
	}

	IRenderTarget* pRT = pLayer->GetRenderTarget();
	HDC hMemDC = pRT->GetHDC();
	POINT  ptOffset = { 0, 0 };

	HRGN hRgn = CreateRectRgn(0,0,0,0);
	int nRet = ::GetWindowRgn(m_hWnd, hRgn);
	if (ERROR == nRet)
	{
		BitBlt(hDC,
			prc->left, 
			prc->top,
			RECTW(prc),
			RECTH(prc), 
			hMemDC, 
			ptOffset.x,
			ptOffset.y, 
			SRCCOPY);
	}
	else
	{
		::SelectClipRgn(hDC, hRgn);
		::OffsetClipRgn(hDC, prc->left, prc->top);

		BitBlt(hDC,
			prc->left, 
			prc->top, 
			RECTW(prc), 
			RECTH(prc),
			hMemDC, 
			ptOffset.x, 
			ptOffset.y,
			SRCCOPY);

		::SelectClipRgn(hDC, nullptr);
	}
	SAFE_DELETE_GDIOBJECT(hRgn);
}

void WindowBase::EnableDwmTransition(bool b)
{
    DwmHelper::GetInstance()->EnableTransition(m_hWnd, b);
}

void WindowBase::SetActive(bool b)
{
    m_objState.activate = b;
}
bool WindowBase::IsActive()
{
	return m_objState.activate;
}

#if 1   // CreateData

void  WindowBase::SetFocusObject(Object* pObj)
{
    m_oMouseManager.SetFocusObject(pObj);
}

// 获取当前鼠标下的对象 
Object*  WindowBase::GetObjectUnderCursor()
{
    POINT  pt;
    GetCursorPos(&pt);
    ::ScreenToClient(m_hWnd, &pt);

    return m_oMouseManager.GetObjectByPos(this, &pt, nullptr);
}
Object*  WindowBase::GetObjectByPos(Object* pObjParent, POINT* pt, bool bSkinBuilderInvoke)
{
    if (bSkinBuilderInvoke)
	{
        return m_oMouseManager.GetObjectByPos_UIEditor(pObjParent, pt, nullptr);
	}
    else
	{
        return m_oMouseManager.GetObjectByPos(pObjParent, pt, nullptr);
	}
}
#endif

// 不负责刷新
void WindowBase::OnSetDefId(IObject* pButton)
{
    if (pButton)
        m_oMouseManager.SetOriginDefaultObject(pButton->GetImpl());
    else
        m_oMouseManager.SetOriginDefaultObject(nullptr);
}

IObject* WindowBase::OnGetDefId()
{
    Object* pObj = m_oMouseManager.GetOriginDefaultObject();
    if (pObj)
    {
        return pObj->GetIObject();
    }
    else
    {
        return nullptr;
    }
}

IRenderFont*  WindowBase::GetWindowDefaultRenderFont() 
{
	return m_pDefaultFont; 
}

void  WindowBase::SetDefaultRenderFont(const wchar_t* szFontId)
{
	SAFE_RELEASE(m_pDefaultFont);
    if (!szFontId)
        return;

	FontRes* pFontRes = nullptr;
	if (m_pSkinRes)
		pFontRes = &m_pSkinRes->GetFontRes();

	if (nullptr == pFontRes)
		return;

    // 默认，如果传入空字符串，则直接创建默认字体
    if (szFontId[0] == TEXT('\0'))
    {
        pFontRes->GetDefaultFont(m_oWindowRender.GetGraphicsRenderType(), &m_pDefaultFont);
    }
    else
    {
	    pFontRes->GetFont(szFontId, m_oWindowRender.GetGraphicsRenderType(), &m_pDefaultFont);
    }

	// 创建默认字体
	if (nullptr == m_pDefaultFont)
	{
		// GDI 窗口字体
		HFONT hFont = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0,0);
		if (hFont)
		{
			UI_AttachFont(&m_pDefaultFont, hFont, m_oWindowRender.GetGraphicsRenderType());
		}
		else
		{
			// UI Font Res Defualt Font
			pFontRes->GetDefaultFont(m_oWindowRender.GetGraphicsRenderType(), &m_pDefaultFont);
			if (nullptr == m_pDefaultFont)
			{
				// System Default Font
				hFont = (HFONT)GetStockObject(SYSTEM_FONT);
				UI_AttachFont(&m_pDefaultFont, hFont, m_oWindowRender.GetGraphicsRenderType());
			}
		}
	}
}

const wchar_t*  WindowBase::GetDefaultRenderFontId()
{
	if (!m_pDefaultFont)
		return nullptr;

	if (m_pDefaultFont->IsAttach())
		return nullptr;
	
    if (!m_pSkinRes)
        return nullptr;

    return m_pSkinRes->GetFontRes().GetRenderFontId(m_pDefaultFont);
}

bool  WindowBase::IsDoModal() 
{ 
    return m_bDoModal;
}
HRESULT  WindowBase::SetCanDrop(bool b)
{
    return m_oDragDropManager.SetDroppable(b); 
}

void  WindowBase::SetObjectPos( int x, int y, int cx, int cy, int nFlag)
{
	// 对于窗口来说，这里设置的是非客户区的大小
	::SetWindowPos(m_hWnd, HWND_TOP, x, y, cx, cy, SWP_NOACTIVATE);
	::GetClientRect(m_hWnd, &m_rcParent);
}

void  WindowBase::virtualSetVisibleEx(VISIBILITY_TYPE eType)
{
	bool bVisibleCompatible = eType==VISIBILITY_VISIBLE ? true:false;
	::ShowWindow(m_hWnd, bVisibleCompatible?SW_SHOW:SW_HIDE);
}

void  WindowBase::virtualSetEnable(bool b)
{
	::EnableWindow(m_hWnd, b ? TRUE:FALSE);
}

bool  WindowBase::IsVisible()
{
    // TBD: 在编辑模式下，被编辑的窗口是隐藏的，导致invalid时判断窗口不可见直接返回了。
    //      所以在这里做一个hack，允许编辑模式下的invalid
    if (GetUIApplication()->IsEditorMode())
        return true;

	return ::IsWindowVisible(((WindowBase*)this)->m_hWnd)?true:false;
}

bool  WindowBase::IsEnable()
{
	return ::IsWindowEnabled(m_hWnd)?true:false;
}

void WindowBase::SetWindowMessageCallback(IWindowDelegate* p)
{
	if (m_pCallbackProxy)
	{
		this->RemoveHook(m_pCallbackProxy);
		SAFE_DELETE(m_pCallbackProxy);
	}

	if (!p)
		return;

	m_pCallbackProxy = new CWindowMessageHookProxy(p);
	this->AddHook(m_pCallbackProxy, 0, 0);
}


bool UI::WindowBase::IsSizeMoveIng()
{
	return m_syncWindow.IsSizeMoveIng();
}



//
//	将一个对象的子对象打散，全放在一个list中用于重新布局
//
//  注：为了解决将两在两个布局描述文件中的控件对应起来，这里采用的方法为：使用id匹配
//     1. 没有设置id的控件将被忽略。
//     2. 对于通用控件，例如combobox->edit/button，采用comboboxid.edit来命名,但查找是用guid来查找的
//     3. 对于滚动条，皮肤中设置id，在查找完了手动将id清掉，避免重名
//     4. 对于listbox.edit，没有设置id，采用guid来查找
void  GetNamedChildrenAndReleaseOthers(
		Object* pParent, map<String, Object*>& mapObjects)
{
	if (nullptr == pParent)
		return;

	// 趁对象还在树结构上面，取消各种绑定关系, 避免崩溃
	UISendMessage(pParent->GetIObject(), UI_MSG_DOBINDPLZ, false);

	Object*  pNext = nullptr;
	Object*  pChild = pParent->EnumAllChildObject(nullptr);
	while (pChild)
	{
		// 从底向上进行逻辑处理
		GetNamedChildrenAndReleaseOthers(pChild, mapObjects);

		// 独立出来，销毁
		pNext = pParent->EnumAllChildObject(pChild);  // save
		pChild->ClearMyTreeRelationOnly();

		const wchar_t* id = pChild->GetId();
		if (id && id[0])
		{
#ifdef _DEBUG
			// 重名检测 
			auto iter = mapObjects.find(String(id));
			if (iter != mapObjects.end())
			{
				DebugBreak();
			}
#endif
			// 保存起来
			mapObjects[String(id)] = pChild;
		}
		else
		{
			// 直接销毁
			pChild->GetIObject()->Release();
		}

		pChild = pNext;
	}
}

// 更换窗口布局，用在一些需要改变布局的场景。
// 场景1：会话窗口，开启桌面共享后，整个界面布局需要进行调整。这种情况下如果使用代码进行调整会很麻烦，而且有些控件样式也不一样。
//
void  WindowBase::ChangeSkinLayout(const wchar_t* szLayoutId)
{
	// 1. 清理窗口状态
	if (m_pCallbackProxy)
		m_pCallbackProxy->DoBindPlz(false);

	this->m_oMouseManager.ClearStateDirect();

	// 2. 遍历所有控件，保留带guid属性的控件，其它控件全部销毁
	map<String, Object*> listObjectsWithName;
	GetNamedChildrenAndReleaseOthers(this, listObjectsWithName);
	this->ClearMyTreeRelationOnly();

	// 设置窗口的新Id，用于重新加载
	m_pSkinRes->GetLayoutManager().ReLoadLayout(
		this, szLayoutId, listObjectsWithName);

	// 将剩余没有再被使用的object释放掉
	auto iter3 = listObjectsWithName.begin();
	for ( ; iter3 != listObjectsWithName.end(); ++iter3)
	{
		Object* pObj = iter3->second;
		pObj->GetIObject()->Release();
	}

	m_oWindowRender.SetCanCommit(false); 
	{
		ForwardInitializeMessageToDecendant(this);

		if (m_pLayout)
		{
			m_pLayout->SetDirty(true);
		}
		this->Invalidate();

		if (m_pCallbackProxy)
		{
			m_pCallbackProxy->DoBindPlz(true);
		}

		// 调整窗口大小，以满足最小尺寸
		SIZE minSize = {GetMinWidth(), GetMinHeight()};

		CRect rcWnd;
		GetWindowNormalRect(&rcWnd);

		bool bNeedResize = false;
		if (rcWnd.Width() < minSize.cx)
		{
			rcWnd.right = rcWnd.left + minSize.cx;
			bNeedResize = true;
		}
		if (rcWnd.Height() < minSize.cy)
		{
			rcWnd.bottom = rcWnd.top + minSize.cy;
			bNeedResize = true;
		}

		if (bNeedResize)
		{
			SetWindowNormalRect(&rcWnd);
		}
	}
	m_oWindowRender.SetCanCommit(true);
}

void  WindowBase::GetWindowNormalRect(LPRECT prc)
{
	UI::GetWindowNormalRect(m_hWnd, prc);
}
void  WindowBase::SetWindowNormalRect(LPCRECT prc)
{
	UIASSERT(prc);

	RECT rc = *prc;
	SIZE minSize = {GetMinWidth(), GetMinHeight()};

	if (RECTW(rc) < minSize.cx)
		rc.right = rc.left + minSize.cx;
	if (RECTH(rc) < minSize.cy)
		rc.bottom = rc.top + minSize.cy;

	if (IsSizeMoveIng())
	{
		::ReleaseCapture();
	
		AsyncTask([rc](WindowBase* _this) 
		{
			HWND hWnd = _this->GetHWND();
			::MoveWindow(hWnd,
				rc.left, rc.top,
				rc.right-rc.left,
				rc.bottom-rc.top, TRUE);
		});
	}
	else
	{
		if (IsIconic(m_hWnd) || IsZoomed(m_hWnd))
		{
			WINDOWPLACEMENT wndplacement = {0};
			wndplacement.length = sizeof(wndplacement);
			GetWindowPlacement(m_hWnd, &wndplacement);

			CopyRect(&wndplacement.rcNormalPosition, &rc);
			::SetWindowPlacement(m_hWnd, &wndplacement);
		}
		else
		{
			::MoveWindow(m_hWnd,
				rc.left, rc.top,
				rc.right-rc.left,
				rc.bottom-rc.top, TRUE);
		}
	}
}