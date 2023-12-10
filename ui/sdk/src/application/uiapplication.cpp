#include "uiapplication.h"

#include "include/interface/iobject.h"
#include "include/interface/iuiautotest.h"
#include "ui/gpu/include/api.h"
#include "src/private_inc.h"
#include "src/resource/colormanager.h"
#include "src/resource/fontmanager.h"
#include "src/resource/i18nmanager.h"
#include "src/resource/imagemanager.h"
#include "src/resource/layoutmanager.h"
#include "src/resource/res_bundle.h"
#include "src/resource/stylemanager.h"
#include "src/skin_parse/skinparseengine.h"
#include "src/window/window_meta.h"
#include "src/panel/panel_meta.h"
#if defined(OS_MAC)
#include "application_mac.h"
#endif

namespace ui {

Application::Application(IApplication *p)
    : m_pUIApplication(p),
      m_TopWindowMgr(this), m_renderBaseFactory(*this),
      m_textRenderFactroy(*this), m_animate(*this), m_resource_manager(*this) {
#if defined(OS_MAC)
  ApplicationMac::Init();
#endif
}
void Application::Run() { m_message_loop.Run(); }
void Application::Quit() { m_message_loop.Quit(); }

ResourceManager &Application::GetResourceManager() {
  return m_resource_manager;
}

void Application::x_Init() {
  UI_LOG_INFO("Application Init: 0x%x", this);

#if 0 // defined(OS_WIN)
  //	::CoInitialize(0);
  HRESULT hr = OleInitialize(0); // 需要注册richedit的drag drop，因此用ole初始化
  (hr);
#endif

  m_bEditorMode = false;
  m_pUIEditor = nullptr;

#if 0
    m_animate.Init(&m_WaitForHandlesMgr);

	m_pGifTimerMgr = nullptr;
	m_pGifTimerMgr = new GifTimerManager();
	m_pGifTimerMgr->Init(this);

    m_ToolTipMgr.Init(this);
#endif

#if defined(OS_WIN)
    // 获取操作系统版本信息
    ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
    m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*) &m_osvi);

    // 设置当前语言。主要是用于 strcoll 中文拼音排序(如：combobox排序)(TODO:这一个是不是需要作成一个配置项？)
    // libo 2017/1/20 在Win10上面调用这个函数会导致内容提交大小增加2M，原因未知。先屏蔽
	// _wsetlocale( LC_ALL, _T("chs") );
#endif
#if 0
    // 初始化Gdiplus
    // 注：gdiplus会创建一个背景线程：GdiPlus.dll!BackgroundThreadProc()  + 0x59 字节	
    Image::InitGDIPlus();

    /* INITIALIZE COMMON CONTROLS, tooltip support */
    INITCOMMONCONTROLSEX iccex; 
    iccex.dwICC = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

    // 针对layer window防止无响应时窗口变黑
    //DisableProcessWindowsGhosting();
#endif
  m_bGpuEnable = false;

  EnableGpuComposite();

  // 先初始化DPI设置，要不然在其它模块在初始化时，直接调用GetDC取到的DPI还是正常值96。
  GetDpi();

  RegisterDefaultUIObject();

  m_pUIAutoTest = nullptr;
#if 0
  RegisterWndClass();
	HMODULE h = LoadLibrary(L"python36.dll");
	// 加载自动化测试模块
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

Resource *Application::GetDefaultSkinRes() {
  return m_resource_manager.GetDefaultSkinRes();
}
MessageLoop &Application::GetMessageLoop() { return m_message_loop; }

Application::~Application(void) {
  // 应用程序退出日志
  UI_LOG_INFO("------------  UI Quit ----------------");

  if (m_pUIAutoTest) {
    m_pUIAutoTest->Release();
  }
#if 0
	this->m_ToolTipMgr.Release();  // 保证顶层窗口计数为0
#endif
#ifdef _DEBUG
  int nCount = this->m_TopWindowMgr.GetTopWindowCount();
  if (0 !=
      nCount) // <--
              // 该方法有可能还是不准，有可能窗口被销毁了，但窗口对象还没有析构
  {
    UI_LOG_WARN("UI_Exit TopWindowCount=%d", nCount);
  }
#endif

  ClearRegisterUIObject();

#if 0
	if (m_WndForwardPostMsg.IsWindow())
	{
		m_WndForwardPostMsg.DestroyWindow();
	}
#endif
  m_resource_manager.Destroy();

  m_pUIEditor = nullptr;

  m_animate.UnInit();

#if 0
	SAFE_DELETE(m_pGifTimerMgr);
    Image::ReleaseGDIPlus();
    if (m_bGpuEnable)
    {
		ShutdownGpuCompositor();
    }
    //	::CoUninitialize(); // do not call CoInitialize, CoInitializeEx, or CoUninitialize from the DllMain function. 
	OleUninitialize();
#endif
}

ITopWindowManager *Application::GetITopWindowMgr() {
  return m_TopWindowMgr.GetITopWindowManager();
}

uia::IAnimateManager *Application::GetAnimateManager() {
  return m_animate.GetAnimateManager();
}

#if 0 // defined(OS_WIN)
//	一个空的窗口过程，因为UI这个窗口类的窗口过程最终要被修改成为一个类的成员函数，
//  因此这里的窗口过程只是用来填充WNDCLASS参数。
//
long CALLBACK WndProc(HWND hWnd, unsigned int message, long wParam,
                      long lParam) {
  return ::DefWindowProc(hWnd, message, wParam, lParam);
}
#endif

#if defined(OS_WIN)

bool Application::IsUnderXpOS() {
  bool bUnderXpOs = true;
  ;
  if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId) {
    if (m_osvi.dwMajorVersion >= 6) {
      bUnderXpOs = false;
    }
  } else {
    bUnderXpOs = false;
  }
  return bUnderXpOs;
}

bool Application::IsVistaOrWin7etc() {
  bool bHighThanVista = true;
  ;
  if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId) {
    if (m_osvi.dwMajorVersion < 6) {
      bHighThanVista = false;
    }
  } else {
    bHighThanVista = false;
  }
  return bHighThanVista;
}
#endif

bool Application::GetSkinTagParseFunc(const char *szTag,
                                      pfnParseSkinTag *pFunc) {
  if (nullptr == szTag || nullptr == pFunc)
    return false;

  UISKINTAGPARSE_DATA::iterator iter = m_mapSkinTagParseData.find(szTag);
  if (iter == m_mapSkinTagParseData.end())
    return false;

  *pFunc = iter->second;
  return true;
}

bool Application::RegisterControlTagParseFunc(const char *szTag,
                                              pfnParseControlTag func) {
  if (nullptr == szTag || nullptr == func)
    return false;

  m_mapControlTagParseData[szTag] = func;
  return true;
}

bool Application::GetControlTagParseFunc(const char *szTag,
                                         pfnParseControlTag *pFunc) {
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
//	注:
//如果仅仅采用在UICreateObject中添加映射列表，无法处理第三方实现一个UI对象的情况，因些
//      必须将该映射列表保存为动态数组。当第三方实现了一个UI类时，向app来注册其创建信息。
//

bool Application::RegisterUIObject(IMeta *pObjDesc) {
  if (!pObjDesc)
    return false;

  int nSize = (int)m_vecUIObjectDesc.size();
  for (int i = 0; i < nSize; i++) {
    m_vecUIObjectDesc[i];
    if (m_vecUIObjectDesc[i] == pObjDesc) {
      UI_LOG_WARN("register duplicate. name=%s", pObjDesc->Name());
      return false;
    }
  }

  m_vecUIObjectDesc.push_back(pObjDesc);
  return true;
}

void Application::ClearRegisterUIObject() {
#define vec_clear(type, var)                                                   \
  {                                                                            \
    type::iterator iter = var.begin();                                         \
    for (; iter != var.end(); iter++)                                          \
      SAFE_DELETE(*iter);                                                      \
    var.clear();                                                               \
  }

  m_vecUIObjectDesc.clear();

  m_renderBaseFactory.Clear();
  m_textRenderFactroy.Clear();
  m_layoutFactory.Clear();
}

void Application::RegisterDefaultUIObject() {
  RegisterUIObject(&WindowMeta::Get());
  RegisterUIObject(&PanelMeta::Get());
#if 0
  RegisterUIObject(RoundPanelMeta::Get());
  RegisterUIObject(ScrollPanelMeta::Get());
  RegisterUIObject(CustomWindowMeta::Get());
  RegisterUIObject(HwndHostMeta::Get());
    //  RegisterUIObject(ScrollPanelDescription::Get());
#endif

  m_mapSkinTagParseData[XML_IMG] = ImageManager::UIParseImageTagCallback;
  m_mapSkinTagParseData[XML_COLOR] = ColorManager::UIParseColorTagCallback;
  m_mapSkinTagParseData[XML_FONT] = FontManager::UIParseFontTagCallback;
  m_mapSkinTagParseData[XML_STYLE] = StyleManager::UIParseStyleTagCallback;
  m_mapSkinTagParseData[XML_LAYOUT] = LayoutManager::UIParseLayoutTagCallback;
  m_mapSkinTagParseData[XML_LAYOUTCONFIG] =
      LayoutManager::UIParseLayoutConfigTagCallback;
  m_mapSkinTagParseData[XML_INCLUDE] =
      SkinParseEngine::UIParseIncludeTagCallback;
  m_mapSkinTagParseData[XML_I18N] = I18nManager::UIParseI18nTagCallback;

  m_renderBaseFactory.Init();
  m_textRenderFactroy.Init();
  m_layoutFactory.Init();
}

// 用于编辑器中调整控件库的依赖
void Application::RestoreRegisterUIObject() {
  ClearRegisterUIObject();
  RegisterDefaultUIObject();
}

IObject *Application::CreateUIObjectByName(const char *szXmlName,
                                           IResource *pSkinRes) {
  if (!szXmlName)
    return nullptr;

  int nSize = (int)m_vecUIObjectDesc.size();
  for (int i = 0; i < nSize; i++) {
    if (0 == strcmp(szXmlName, m_vecUIObjectDesc[i]->Name())) {
      IObject *p = nullptr;
      m_vecUIObjectDesc[i]->Create(pSkinRes, (void **)&p);
      return p;
    }
  }

  UI_LOG_ERROR("GetUICreateInstanceFuncPtr Failed. name=%s", szXmlName);
  return nullptr;
}

IObject *Application::CreateUIObjectByClsid(const Uuid &clsid,
                                            IResource *pSkinRes) {
  int nSize = (int)m_vecUIObjectDesc.size();
  for (int i = 0; i < nSize; i++) {
    if (clsid == m_vecUIObjectDesc[i]->UUID()) {
      IObject *p = nullptr;
      m_vecUIObjectDesc[i]->Create(pSkinRes, (void **)&p);
      return p;
    }
  }

  UI_LOG_ERROR("GetUICreateInstanceFuncPtr Failed.");
  return nullptr;
}
#if 0
 
void  Application::MsgHandleOnce()
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


bool  Application::ShowToolTip(TOOLTIPITEM* pItem)
{
	bool bRet = m_ToolTipMgr.Show(pItem);
    return bRet;
}
void  Application::HideToolTip()
{
	m_ToolTipMgr.Hide();
}

HWND  Application::GetForwardPostMessageWnd()
{
	return m_WndForwardPostMsg.m_hWnd;
}

// 加载UI3D.dll
HMODULE  Application::GetUID2DModule()
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
        UI_LOG_INFOA("%s Load UI3D.dll Failed. Error code = %d"), FUNC_NAME, GetLastError();
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

HMODULE  Application::GetUID3DModule()
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
#endif

bool Application::IsGpuCompositeEnable() { return m_bGpuEnable; }
bool Application::EnableGpuComposite() {
  if (m_bGpuEnable)
    return true;
#if defined(OS_WIN)
  if (!IsVistaOrWin7etc()) {
    UI_LOG_ERROR(TEXT("EnableGpuComposite Failed. OS Version mistake"));
    return false;
  }

  HMODULE hModule = LoadLibrary(L"uigpu.dll");
  if (!hModule) {
    UI_LOG_ERROR(TEXT("LoadLibrary UICompositor Failed. Error=%d"),
                 GetLastError());
    return false;
  }

  typedef bool (*pfnUIStartupGpuCompositor)();
  pfnUIStartupGpuCompositor fn = (pfnUIStartupGpuCompositor)::GetProcAddress(
      hModule, "GpuStartup");

  if (!fn) {
    UI_LOG_ERROR(TEXT("UIStartupGpuCompositor Failed"));
    return false;
  }

  m_bGpuEnable = fn();
  UI_LOG_INFO("GpuCompositor Enable.");
#else
  m_bGpuEnable = ui::GpuStartup();
#endif
  if (!m_bGpuEnable) {
    UI_LOG_WARN("gpu startup failed!");
  }
  return true;
}

void Application::ShutdownGpuCompositor() {
  if (!m_bGpuEnable)
    return;

#if defined(OS_WIN)
  HMODULE hModule = GetModuleHandle(L"uigpu.dll");
  if (!hModule)
    return;

  typedef long (*pfnUIShutdownGpuCompositor)();
  pfnUIShutdownGpuCompositor fn = (pfnUIShutdownGpuCompositor)::GetProcAddress(
      hModule, "GpuShutdown");

  if (!fn)
    return;

  fn();
#else
  ui::GpuShutdown();
#endif
  m_bGpuEnable = false;
}

void Application::LoadUIObjectListToToolBox() {
  if (!m_pUIEditor)
    return;

  UIOBJ_CREATE_DATA::iterator iter = m_vecUIObjectDesc.begin();
  for (; iter != m_vecUIObjectDesc.end(); iter++) {
    m_pUIEditor->OnToolBox_AddObject((*iter));
  }
}

bool Application::CreateRenderBaseByName(const char *szName,
                                         IObject *pObject,
                                         IRenderBase **ppOut) {
  IResource *pSkinRes = nullptr;
  if (pObject) {
    pSkinRes = pObject->GetResource();
  } else {
    Resource *p = GetDefaultSkinRes();
    pSkinRes = p ? p->GetIResource() : nullptr;
  }

  return m_renderBaseFactory.CreateRenderBaseByName(pSkinRes, szName, pObject,
                                                    ppOut);
}

const char *Application::GetRenderBaseName(int nType) {
  return m_renderBaseFactory.GetRenderBaseName(nType);
}

} // namespace ui
