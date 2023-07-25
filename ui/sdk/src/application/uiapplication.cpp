#include "uiapplication.h"

#include "inc.h"
#include "private_inc.h"
#include "include/interface/iobject.h"
#include "src/resource/imagemanager.h"
#include "src/resource/colormanager.h"
#include "src/resource/fontmanager.h"
#include "src/resource/stylemanager.h"
#include "src/resource/layoutmanager.h"
#include "src/resource/i18nmanager.h"
#include "src/resource/skinres.h"
#include "src/skin_parse/skinparseengine.h"

#if defined(OS_MAC)
#include "application_mac.h"
#endif

namespace ui {


UIApplication::UIApplication(IUIApplication* p): 
    m_pUIApplication(p),
    // m_WndForwardPostMsg(this), 
    // m_TopWindowMgr(this),
	m_renderBaseFactory(*this),
	m_textRenderFactroy(*this)
	//, m_animate(*this) 
{
#if defined(OS_MAC)
  ApplicationMac::Init();
#endif
}
void UIApplication::Run() { m_message_loop.Run(); }
void UIApplication::Quit() { m_message_loop.Quit(); }


SkinManager& UIApplication::GetSkinManager()
{
	return m_skin_manager;
}


void  UIApplication::x_Init()
{
#if defined(OS_WIN)
    //	::CoInitialize(0);
    HRESULT  hr = OleInitialize(0);  // 需要注册richedit的drag drop，因此用ole初始化
	(hr);
#endif

#if 0
    m_bEditorMode = false;
    m_pUIEditor = nullptr;

    m_skin_manager.SetUIApplication(this);
    m_animate.Init(&m_WaitForHandlesMgr);

	m_pGifTimerMgr = nullptr;
	m_pGifTimerMgr = new GifTimerManager();
	m_pGifTimerMgr->Init(this);

    m_ToolTipMgr.Init(this);

    // 获取操作系统版本信息
    ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
    m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*) &m_osvi);

    // 设置当前语言。主要是用于 strcoll 中文拼音排序(如：combobox排序)(TODO:这一个是不是需要作成一个配置项？)
    // libo 2017/1/20 在Win10上面调用这个函数会导致内容提交大小增加2M，原因未知。先屏蔽
	// _wsetlocale( LC_ALL, _T("chs") );

    // 初始化Gdiplus
    // 注：gdiplus会创建一个背景线程：GdiPlus.dll!BackgroundThreadProc()  + 0x59 字节	
    Image::InitGDIPlus();

    /* INITIALIZE COMMON CONTROLS, tooltip support */
    INITCOMMONCONTROLSEX iccex; 
    iccex.dwICC = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

    // 创建一个用于转发消息的窗口，实现post ui message
    m_WndForwardPostMsg.Create(HWND_MESSAGE);

    // 针对layer window防止无响应时窗口变黑
    //DisableProcessWindowsGhosting();

    m_bGpuEnable = false;

    // 先初始化DPI设置，要不然在其它模块在初始化时，直接调用GetDC取到的DPI还是正常值96。
    GetDpi();

    RegisterDefaultUIObject(); 
    RegisterWndClass();
	
	HMODULE h = LoadLibrary(L"python36.dll");
	// 加载自动化测试模块
	m_pUIAutoTest = nullptr;
	HMODULE hAutoTest = ::LoadLibrary(TEXT("UIAutomator.dll"));
	if (hAutoTest)
	{
		typedef IUIAutoTest*  (*pfnGetUIAutoTest)();
		pfnGetUIAutoTest fn = (pfnGetUIAutoTest)
			::GetProcAddress(hAutoTest, "GetUIAutoTest");

		if (fn) {
			m_pUIAutoTest = fn();
		}
		if (m_pUIAutoTest) {
			m_pUIAutoTest->Init();
		}
	}
#endif
}

SkinRes* UIApplication::GetDefaultSkinRes()
{
	return m_skin_manager.GetDefaultSkinRes();
}

#if 0
UIApplication::~UIApplication(void)
{
	// 应用程序退出日志
	UI_LOG_INFO( _T("\n\n------------  UI Quit ----------------\n"));

	if (m_pUIAutoTest) {
		m_pUIAutoTest->Release();
	}

	this->m_ToolTipMgr.Release();  // 保证顶层窗口计数为0

#ifdef _DEBUG
	int nCount = this->m_TopWindowMgr.GetTopWindowCount();
	if (0 != nCount)   // <-- 该方法有可能还是不准，有可能窗口被销毁了，但窗口对象还没有析构
	{
		UI_LOG_WARN(_T("UI_Exit TopWindowCount=%d"),nCount);  
	}
#endif

    ClearRegisterUIObject();

	if (m_WndForwardPostMsg.IsWindow())
	{
		m_WndForwardPostMsg.DestroyWindow();
	}
	m_skin_manager.Destroy();

	m_pUIEditor = nullptr;

    m_animate.UnInit();

	SAFE_DELETE(m_pGifTimerMgr);

    Image::ReleaseGDIPlus();

    if (m_bGpuEnable)
    {
		ShutdownGpuCompositor();
    }

    //	::CoUninitialize(); // do not call CoInitialize, CoInitializeEx, or CoUninitialize from the DllMain function. 
	OleUninitialize();
}

ITopWindowManager* UIApplication::GetITopWindowMgr()
{
	return m_TopWindowMgr.GetITopWindowManager();
}

UIA::IAnimateManager* UIApplication::GetAnimateMgr()
{
    return m_animate.GetAnimateManager();
}

//	一个空的窗口过程，因为UI这个窗口类的窗口过程最终要被修改成为一个类的成员函数，
//  因此这里的窗口过程只是用来填充WNDCLASS参数。
//
long CALLBACK WndProc(HWND hWnd, unsigned int message, long wParam, long lParam)
{
	return ::DefWindowProc( hWnd, message, wParam, lParam );
}

/*
**	[private] void  RegisterWndClass()
**
**	Parameters
**
**	Return
**		NA
**
**	Remark
**		Window对象的窗口使用的窗口类就是在这里注册的。
**
**	See Also
*/
void UIApplication::RegisterWndClass()
{
	WNDCLASSEX wcex;
	
	// 注册UI普通窗口类
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= /*CS_HREDRAW | CS_VREDRAW |*/CS_DBLCLKS;  // <-- 分层窗口最大化时收不到redraw消息，因此直接在OnSize中刷新
	wcex.lpfnWndProc	= WindowBase::StartWindowProc;//WndProc;   改用windows默认的窗口过程
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;//::AfxGetInstanceHandle();;
	wcex.hIcon			= 0;//LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1); // 2014.5.21 将背景置为nullptr，解决aero磨砂窗口从最小化还原时，会先显示白色背景，再刷新内容的闪烁问题（触发了WM_NCCALCSIZE内部估计会做COPY操作）
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= WND_CLASS_NAME;
	wcex.hIconSm		= 0;//LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
	RegisterClassEx(&wcex);

	// 注册用于实现动画的窗口类
	wcex.lpszClassName  = WND_ANIMATE_CLASS_NAME;
	wcex.lpfnWndProc    = ::DefWindowProc;
	RegisterClassEx(&wcex);


	// 注册用于实现菜单，弹出式列表框的窗口(带阴影/不带阴影)
	wcex.lpszClassName  = WND_POPUP_CONTROL_SHADOW_NAME;
	wcex.style          = CS_DROPSHADOW;
	wcex.lpfnWndProc    = WindowBase::StartWindowProc;;
	RegisterClassEx(&wcex);

	wcex.lpszClassName  = WND_POPUP_CONTROL_NAME;
	wcex.style          = 0;
	wcex.lpfnWndProc    = WindowBase::StartWindowProc;;
	RegisterClassEx(&wcex);

	// 拖拽时显示的图片窗口
	wcex.lpszClassName  = WND_DRAGBITMAPWND_CLASS_NAME;
	wcex.style          = 0;
	wcex.lpfnWndProc    = DefWindowProc;
	RegisterClassEx(&wcex);

}

bool UIApplication::IsUnderXpOS()
{
	bool bUnderXpOs = true;;
	if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId)
	{
		if (m_osvi.dwMajorVersion >= 6)
		{
			bUnderXpOs = false;
		}
	}
	else
	{
		bUnderXpOs = false;
	}
	return bUnderXpOs;
}

bool UIApplication::IsVistaOrWin7etc()
{
	bool bHighThanVista = true;;
	if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId)
	{
		if (m_osvi.dwMajorVersion < 6)
		{
			bHighThanVista = false;
		}
	}
	else
	{
		bHighThanVista = false;
	}
	return bHighThanVista;
}
#endif
bool  UIApplication::GetSkinTagParseFunc(const wchar_t* szTag, pfnParseSkinTag* pFunc)
{
    if (nullptr == szTag || nullptr == pFunc)
        return false;

    UISKINTAGPARSE_DATA::iterator iter = m_mapSkinTagParseData.find(szTag);
    if (iter == m_mapSkinTagParseData.end())
        return false;

    *pFunc = iter->second;
    return true;
}

bool  UIApplication::RegisterControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag func)
{
    if (nullptr == szTag || nullptr == func)
        return false;

    m_mapControlTagParseData[szTag] = func;
    return true;
}

bool  UIApplication::GetControlTagParseFunc(const wchar_t* szTag, pfnParseControlTag* pFunc)
{
    if (nullptr == szTag || nullptr == pFunc)
        return false;

    UICONTROLTAGPARSE_DATA::iterator iter = m_mapControlTagParseData.find(szTag);
    if (iter == m_mapControlTagParseData.end())
        return false;

    *pFunc = iter->second;
    return true;
}

//
//	为了实现UI对象的创建（从字符串来创建其对应的类），在app类中保存了所有UI对象的创建信息。
//
//	注: 如果仅仅采用在UICreateObject中添加映射列表，无法处理第三方实现一个UI对象的情况，因些
//      必须将该映射列表保存为动态数组。当第三方实现了一个UI类时，向app来注册其创建信息。
//

bool UIApplication::RegisterUIObject(IObjectDescription* pObjDesc)
{
	if (!pObjDesc)
		return false;
	
	int nSize = (int)m_vecUIObjectDesc.size();
	for (int i = 0; i < nSize; i++)
	{
		m_vecUIObjectDesc[i];
		if (m_vecUIObjectDesc[i] == pObjDesc)
		{
			UI_LOG_WARN(_T("register duplicate. name=%s"), 
                pObjDesc->GetTagName());
			return false;
		}
	}

	m_vecUIObjectDesc.push_back(pObjDesc);
	return true;
}


void  UIApplication::ClearRegisterUIObject()
{
#define vec_clear(type, var)                  \
    {                                         \
        type::iterator iter = var.begin();    \
        for ( ; iter != var.end(); iter++ )   \
            SAFE_DELETE(*iter);               \
        var.clear();                          \
    }

    m_vecUIObjectDesc.clear();

#if 0
	m_renderBaseFactory.Clear();
	m_textRenderFactroy.Clear();
	m_layoutFactory.Clear();
#else
    UIASSERT(0);
#endif
}

void UIApplication::RegisterDefaultUIObject()
{
#if 0
    RegisterUIObject(PanelDescription::Get());
	RegisterUIObject(RoundPanelDescription::Get());
	RegisterUIObject(ScrollPanelDescription::Get());
    RegisterUIObject(WindowDescription::Get());
    RegisterUIObject(CustomWindowDescription::Get());
    RegisterUIObject(HwndHostDescription::Get());
//    RegisterUIObject(ScrollPanelDescription::Get());
#else
    UIASSERT(0);
#endif

    m_mapSkinTagParseData[XML_IMG] = ImageManager::UIParseImageTagCallback;
    m_mapSkinTagParseData[XML_COLOR] = ColorManager::UIParseColorTagCallback;
    m_mapSkinTagParseData[XML_FONT] = FontManager::UIParseFontTagCallback;
    m_mapSkinTagParseData[XML_STYLE] = StyleManager::UIParseStyleTagCallback;
    m_mapSkinTagParseData[XML_LAYOUT] = LayoutManager::UIParseLayoutTagCallback;
	m_mapSkinTagParseData[XML_LAYOUTCONFIG] = LayoutManager::UIParseLayoutConfigTagCallback;
    m_mapSkinTagParseData[XML_INCLUDE] = SkinParseEngine::UIParseIncludeTagCallback;
	m_mapSkinTagParseData[XML_I18N] = I18nManager::UIParseI18nTagCallback;

#if 0
	m_renderBaseFactory.Init();
	m_textRenderFactroy.Init();
	m_layoutFactory.Init();
#else
    UIASSERT(0);
#endif
}

// 用于编辑器中调整控件库的依赖
void  UIApplication::RestoreRegisterUIObject()
{
    ClearRegisterUIObject();
    RegisterDefaultUIObject();
}

IObject* UIApplication::CreateUIObjectByName(const wchar_t* szXmlName, ISkinRes* pSkinRes)
{
	if (!szXmlName)
		return nullptr;

    int nSize = (int)m_vecUIObjectDesc.size();
    for (int i = 0; i < nSize; i++)
    {
        if (0 == wcscmp(szXmlName, m_vecUIObjectDesc[i]->GetTagName()))
        {
            IObject* p = nullptr;
            m_vecUIObjectDesc[i]->CreateInstance(pSkinRes, (void**)&p);
            return p;
        }
    }

    UI_LOG_ERROR(_T("GetUICreateInstanceFuncPtr Failed. name=%s"), szXmlName);
	return nullptr;
}

#if 0
IObject* UIApplication::CreateUIObjectByClsid(REFCLSID clsid, ISkinRes* pSkinRes)
{
    int nSize = (int)m_vecUIObjectDesc.size();
    for (int i = 0; i < nSize; i++)
    {
        if (::IsEqualIID(clsid, m_vecUIObjectDesc[i]->GetGUID()))
        {
            IObject* p = nullptr;
            m_vecUIObjectDesc[i]->CreateInstance(pSkinRes, (void**)&p);
            return p;
        }
    }

    UI_LOG_ERROR(_T("GetUICreateInstanceFuncPtr Failed."));
    return nullptr;
}

BOOL UIApplication::IsDialogMessage(MSG* pMsg)
{
	if (nullptr == pMsg)
		return false;

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < (WM_MOUSEFIRST+1) || pMsg->message > WM_MOUSELAST))
		return false;

//	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		// 获取这个消息的窗口所在的顶层窗口，因为导航是针对是整个顶层窗口进行的
        // 2013.6.20 屏蔽掉这些代码。存在子窗口是UI窗口，但顶层窗口是普通窗口的情况
 		HWND hWndTop = pMsg->hwnd;
// 		while (1)
// 		{
// 			if (nullptr == hWndTop)
// 				return false;
// 
// 			LONG lStyle = ::GetWindowLongPtr(hWndTop, GWL_STYLE);
// 			if (lStyle & WS_CHILD)
// 			{
// 				hWndTop = ::GetParent(hWndTop);
// 			}
// 			else
// 			{
// 				break;
// 			}
// 		}

		// 判断这个窗口是否属性UI管理的一个顶层窗口
		IWindowBase* pIWindow = m_TopWindowMgr.GetWindowBase(hWndTop);
        if (nullptr == pIWindow)
            return false;

        WindowBase* pWindow = pIWindow->GetImpl();
        if (!pWindow)
            return false;

        return pWindow->GetMouseMgr()->IsDialogMessage(pMsg);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//
// http://dsdm.bokee.com/6033955.html  如何正确使用PeekMessage
// 这里要注意的是多重模态的问题，如何保证正常退出
//
//////////////////////////////////////////////////////////////////////////
// 2014.2.27 libo
// 注意，不要使用MsgWaitForMultipleObjects -- QS_ALLEVENTS标志，会出现系统卡顿现象，例如安装了底层键盘钩子时，一输入就卡了
//
// HHOOK  g_hKeyBoardHook = nullptr;
// long CALLBACK  LLKeyboardProc(int code, long wParam, long lParam)
// {
//     return 0;
//     return CallNextHookEx(g_hKeyBoardHook, code, wParam, lParam);
// }
// hKeyBoardHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, g_hInstance, nullptr);
// 
//////////////////////////////////////////////////////////////////////////


//
//  WaitMessage 使用需知：
//  A pretty hard rule for WaitMessage() is that the message queue should be empty before you call it. 
//  If it isn't empty then any messages left in the queue are marked as "seen" and WaitMessage() ignores them.
// 
//  如果没有新的消息，WaitMessage不会返回。PeekMessage/GetMessage等函数会将现有
//  消息队列中的消息标识为旧消息，因此
//  PostMessage(xxx);
//  PeekMessage(PM_NOREMOVE);
//  WaitMessage();  这里并不会立即返回，xxx消息已被标识为旧消息。除非有一个新的
//  消息到来才能使WaitMessage返回。
//
void  UIApplication::MsgHandleLoop(bool* pbQuitLoopRef)
{
	unsigned int    dwRet = 0;
    unsigned int&   nCount = m_WaitForHandlesMgr.m_nHandleCount;
    HANDLE*& pHandles = m_WaitForHandlesMgr.m_pHandles;
	MSG      msg;

    // 会传递pbQuitLoopRef参数的，有可能是Modal类型的菜单，这种情况下需要更多的条件判断
    // 因此单独用一个分支来优化
    if (pbQuitLoopRef)
    {
        bool   bExit = false;
        bool&  bQuitRef = (pbQuitLoopRef==nullptr ? bExit : *pbQuitLoopRef);
	    while (false == bQuitRef)
	    {
		    dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, false, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
		    if (nCount > 0 && dwRet < nCount)
		    {
			    m_WaitForHandlesMgr.Do(pHandles[dwRet]);
		    }
		    else
		    {
			    while (::PeekMessage(&msg, nullptr, 0,0, PM_NOREMOVE))
			    {
				    if (WM_QUIT == msg.message)  // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
					    return;

                    if (m_MsgFilterMgr.PreTranslateMessage(&msg))  // 有可能在PreTranslateMessage中修改了pbQuitLoopRef
                    {
                        PeekMessage(&msg, nullptr, 0,0, PM_REMOVE);   // 该消息已被处理过了，移除
                        continue;
                    }

                    if (bQuitRef)     // 可能的场景是弹出模态菜单后，然后在主窗口的关闭按钮上点了一下，然后菜单的PopupMenuWindow::PreTranslateMessage就要求退出了
                        return;  // 如果这时还继续处理，就会导致主窗口被销毁，当前用于弹出菜单的堆栈对象也被销毁了

                    PeekMessage(&msg, nullptr, 0,0, PM_REMOVE);
                    if (false == this->IsDialogMessage(&msg))
                    {
                        ::TranslateMessage(&msg);
                        ::DispatchMessage(&msg);
                    }

                    if (bQuitRef)
                        return;
			    }
		    }
	    }
    }
    else
    {
        while (1)
        {
            dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, false, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
            if (nCount > 0 && dwRet < nCount)
            {
                m_WaitForHandlesMgr.Do(pHandles[dwRet]);
            }
            else
            {
                while (::PeekMessage(&msg, nullptr, 0,0, PM_REMOVE))
                {
                    if (WM_QUIT == msg.message)  
                    {
                        ::PostMessage(nullptr, WM_QUIT, 0, 0);  // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
                        return;
                    }

                    if (false == m_MsgFilterMgr.PreTranslateMessage(&msg) && 
                        false == this->IsDialogMessage(&msg)) 
                    {
                        ::TranslateMessage(&msg);                                      
                        ::DispatchMessage(&msg);
                    }
                }
            }
        }
    }
	return;
}

void  UIApplication::MsgHandleOnce()
{
	MSG msg = {0};
	while (::PeekMessage(&msg, nullptr, 0,0, PM_REMOVE))
	{
		if (WM_QUIT == msg.message)  
		{
			::PostMessage(nullptr, WM_QUIT, 0, 0);  // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
			return;
		}

		if (false == m_MsgFilterMgr.PreTranslateMessage(&msg) && 
			false == this->IsDialogMessage(&msg)) 
		{
			::TranslateMessage(&msg);                                      
			::DispatchMessage(&msg);
		}
	}
}


bool  UIApplication::ShowToolTip(TOOLTIPITEM* pItem)
{
	bool bRet = m_ToolTipMgr.Show(pItem);
    return bRet;
}
void  UIApplication::HideToolTip()
{
	m_ToolTipMgr.Hide();
}

HWND  UIApplication::GetForwardPostMessageWnd()
{
	return m_WndForwardPostMsg.m_hWnd;
}

void UIApplication::LoadUIObjectListToToolBox()
{
    if (!m_pUIEditor)
        return;

    UIOBJ_CREATE_DATA::iterator iter = m_vecUIObjectDesc.begin();
    for (; iter != m_vecUIObjectDesc.end(); iter++)
    {
        m_pUIEditor->OnToolBox_AddObject((*iter));
    }
}

// 加载UI3D.dll
HMODULE  UIApplication::GetUID2DModule()
{
#if 0
    if (s_hUID2D)
        return s_hUID2D;

    wchar_t  szPath[MAX_PATH] = {0};
    util::GetAppPath_(g_hInstance, szPath);
    _tcscat(szPath, _T("UI3D.dll"));

    s_hUID2D = LoadLibrary(szPath);

    if (!s_hUID2D)
    {
        UI_LOG_INFO(_T("%s Load UI3D.dll Failed. Error code = %d"), FUNC_NAME, GetLastError());
    }
    else
    {
        funcUI3D_InitD2D pFunc = (funcUI3D_InitD2D)GetProcAddress(s_hUID2D, NAME_UI3D_InitD2D);
        if (pFunc)
            pFunc(m_pUIApplication);
        else
            UIASSERT(0);
    }

	return s_hUID2D;
#endif
	return 0;
}

HMODULE  UIApplication::GetUID3DModule()
{
#if 0
	if (s_hUID3D)
		return s_hUID3D;

	wchar_t  szPath[MAX_PATH] = {0};
	util::GetAppPath_(g_hInstance, szPath);
	_tcscat(szPath, _T("UI3D.dll"));

	s_hUID3D = LoadLibrary(szPath);

	if (!s_hUID3D)
	{
		UI_LOG_INFO(_T("%s Load UI3D.dll Failed. Error code = %d"), FUNC_NAME, GetLastError());
	}
	else
	{
		funcUI3D_InitD3D pFunc = (funcUI3D_InitD3D)GetProcAddress(s_hUID3D, NAME_UI3D_InitD3D);
		if (pFunc)
			pFunc(m_pUIApplication);
		else
			UIASSERT(0);
	}

	return s_hUID3D;
#endif
	return 0;
}

bool  UIApplication::IsGpuCompositeEnable() 
{ 
    return m_bGpuEnable;    
}
bool  UIApplication::EnableGpuComposite()
{
    if (m_bGpuEnable)
        return true;

    if (!IsVistaOrWin7etc())
	{
		UI_LOG_ERROR(TEXT("EnableGpuComposite Failed. OS Version mistake"));
		return false;
	}

	HMODULE hModule = LoadLibrary(L"UICompositor.dll");
	if (!hModule)
	{
		UI_LOG_ERROR(TEXT("LoadLibrary UICompositor Failed. Error=%d"),
			GetLastError());
		return false;
	}

	typedef long  (*pfnUIStartupGpuCompositor)();
	pfnUIStartupGpuCompositor fn = (pfnUIStartupGpuCompositor)
		::GetProcAddress(hModule, "UIStartupGpuCompositor");

	if (!fn)
	{
		UI_LOG_ERROR(TEXT("UIStartupGpuCompositor Failed"));
		return false;
	}

	fn();
	UI_LOG_INFO(TEXT("GpuCompositor Enable."));

	m_bGpuEnable = true;
	return true;
}

void UI::UIApplication::ShutdownGpuCompositor()
{
	if (!m_bGpuEnable)
		return;

	HMODULE hModule = GetModuleHandle(L"UICompositor.dll");
	if (!hModule)
		return;

	typedef long(*pfnUIShutdownGpuCompositor)();
	pfnUIShutdownGpuCompositor fn = (pfnUIShutdownGpuCompositor)
		::GetProcAddress(hModule, "UIShutdownGpuCompositor");

	if (!fn)
		return;

	fn();
	m_bGpuEnable = false;
}
#endif

bool  UIApplication::CreateRenderBaseByName(
		const wchar_t* szName, IObject* pObject, IRenderBase** ppOut)
{
    ISkinRes* pSkinRes = nullptr;
	if (pObject)
	{
		pSkinRes = pObject->GetSkinRes();
	}
	else
	{
		SkinRes* p = GetDefaultSkinRes();
		pSkinRes = p ? p->GetISkinRes() : nullptr;
	}

    return m_renderBaseFactory.CreateRenderBaseByName(
		pSkinRes, szName, pObject, ppOut); 
}

const wchar_t*  UIApplication::GetRenderBaseName(int nType)
{
	return m_renderBaseFactory.GetRenderBaseName(nType);
}

} // namespace ui
