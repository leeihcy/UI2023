#include "uiapplication.h"

#include "include/interface/ianimate.h"
#include "include/interface/iobject.h"
#include "include/interface/irenderbase.h"
#include "include/interface/iuiautotest.h"
#include "include/interface/iuieditor.h"
#include "include/util/log.h"
#include "src/application/config/config.h"
#include "src/animate/animate.h"
#include "src/control/control_meta.h"
#include "src/control/text/text_meta.h"
#include "src/control/button/button_meta.h"
#include "src/graphics/record/record_render_target.h"
#include "src/panel/panel_meta.h"
#include "src/resource/colormanager.h"
#include "src/resource/layoutmanager.h"
#include "src/resource/res_bundle.h"
#include "src/parser/skinparseengine.h"
#include "src/window/window_meta.h"
#include "ui/gpu/include/api.h"

#if !defined(OS_WIN)
#include <libgen.h> // dirname
#endif

#if defined(OS_MAC)
#include "application_mac.h"
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <unistd.h>      // chdir
#endif

namespace ui {

Application::Application(IApplication *p)
    : m_pUIApplication(p), m_TopWindowMgr(this), m_render_base_factory(*this),
      m_text_render_base_factory(*this), m_resource_manager(*this),
      m_message_loop(*this), m_timer_helper(*this), m_animate(this) {
#if defined(OS_MAC)
  ApplicationMac::Init();
#endif
}
void Application::Run() { m_message_loop.Run(); }
void Application::Quit() {
  m_message_loop.Quit();

  if (Config::GetInstance().enable_render_thread) {
    RenderThread::GetIntance().main.Stop();
  }
}

ResourceManager &Application::GetResourceManager() {
  return m_resource_manager;
}

// 切换当前目录为程序所在目录，避免使用相对目录加载皮肤时失败。
static void FixWorkDir() {
#if defined(OS_WIN)
  char exe_dir[MAX_PATH] = {0};
  GetModuleFileNameA(NULL, exe_dir, MAX_PATH);
  char *p = strrchr(exe_dir, '\\');
  if (p) {
    *(p + 1) = 0;
  }
  ::SetCurrentDirectoryA(exe_dir);
#elif defined(OS_LINUX)
  char exe_path[PATH_MAX] = {0};
  char *dir_path;
  if (readlink("/proc/self/exe", exe_path, PATH_MAX) == -1) {
    return;
  }
  dir_path = dirname(exe_path);
  UI_LOG_INFO("chdir to %s", dir_path);

  chdir(dir_path);
#else
  char path[PATH_MAX];
  uint32_t size = sizeof(path);

  // 获取可执行文件路径（macOS 专用）
  if (_NSGetExecutablePath(path, &size) != 0) {
    return;
  }

  // 解析真实路径（处理符号链接）
  char resolved_path[PATH_MAX];
  if (realpath(path, resolved_path) == nullptr) {
    return;
  }
  char *dir = dirname(resolved_path);
  chdir(dir);
#endif
}

void Application::x_Init() {
  UI_LOG_INFO("Application Init: 0x%x", this);
  FixWorkDir();

  m_bEditorMode = false;
  m_pUIEditor = nullptr;

  if (Config::GetInstance().enable_render_thread) {
    RenderThread::GetIntance().main.Start();
  }

#if defined(OS_WIN)
  // 获取操作系统版本信息
  ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
  m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((OSVERSIONINFO *)&m_osvi);
#endif

#if 0
	m_pGifTimerMgr = nullptr;
	m_pGifTimerMgr = new GifTimerManager();
	m_pGifTimerMgr->Init(this);

    m_ToolTipMgr.Init(this);
#endif

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

ResourceBundle *Application::GetDefaultSkinRes() {
  return m_resource_manager.GetDefaultSkinRes();
}
MessageLoop &Application::GetMessageLoop() { return m_message_loop; }

Application::~Application(void) {
  // 应用程序退出日志
  UI_LOG_INFO("\n\n------------  UI Quit ----------------");

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

uia::IAnimate *Application::GetAnimate() { return m_animate.GetIAnimate(); }
void Application::CreateAnimateTimer(int fps) {
  m_message_loop.CreateAnimateTimer(fps);
}
void Application::DestroyAnimateTimer() {
  m_message_loop.DestroyAnimateTimer();
}
void Application::OnAnimateTimer() { m_animate.OnTick(); }

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
                                      pfnParseResourceNode *pFunc) {
  if (nullptr == szTag || nullptr == pFunc)
    return false;

  auto iter = m_resource_node_parser_map.find(szTag);
  if (iter == m_resource_node_parser_map.end())
    return false;

  *pFunc = iter->second;
  return true;
}

bool Application::RegisterControlTagParseFunc(const char *szTag,
                                              pfnParseControlNode func) {
  if (nullptr == szTag || nullptr == func)
    return false;

  m_control_node_parser_map[szTag] = func;
  return true;
}

bool Application::GetControlTagParseFunc(const char *szTag,
                                         pfnParseControlNode *pFunc) {
  if (nullptr == szTag || nullptr == pFunc)
    return false;

  auto iter = m_control_node_parser_map.find(szTag);
  if (iter == m_control_node_parser_map.end())
    return false;

  *pFunc = iter->second;
  return true;
}

//
//	为了实现UI对象的创建（从字符串来创建其对应的类），在app类中保存了所有UI对象的创建信息。
//
//	注:
// 如果仅仅采用在UICreateObject中添加映射列表，无法处理第三方实现一个UI对象的情况，因些
//      必须将该映射列表保存为动态数组。当第三方实现了一个UI类时，向app来注册其创建信息。
//

bool Application::RegisterUIObject(IMeta *pObjDesc) {
  if (!pObjDesc)
    return false;

  int nSize = (int)m_object_meta_array.size();
  for (int i = 0; i < nSize; i++) {
    if (m_object_meta_array[i] == pObjDesc) {
      UI_LOG_WARN("register duplicate. name=%s", pObjDesc->Name());
      return false;
    }
  }

  m_object_meta_array.push_back(pObjDesc);
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

  m_object_meta_array.clear();

  m_render_base_factory.Clear();
  m_text_render_base_factory.Clear();
  m_layout_factory.Clear();
}

void Application::RegisterDefaultUIObject() {
  RegisterUIObject(&WindowMeta::Get());
  RegisterUIObject(&PanelMeta::Get());
  RegisterUIObject(&ControlMeta::Get());
  RegisterUIObject(&RoundPanelMeta::Get());
  RegisterUIObject(&LabelMeta::Get());
  RegisterUIObject(&ButtonMeta::Get());
#if 0
  RegisterUIObject(ScrollPanelMeta::Get());
  RegisterUIObject(CustomWindowMeta::Get());
  RegisterUIObject(HwndHostMeta::Get());
    //  RegisterUIObject(ScrollPanelDescription::Get());
#endif

  m_resource_node_parser_map[XML_COLOR] = ColorManager::UIParseColorTagCallback;
  // m_resource_node_parser_map[XML_FONT] = FontManager::UIParseFontTagCallback;
  m_resource_node_parser_map[XML_STYLE] = StyleManager::UIParseStyleTagCallback;
  m_resource_node_parser_map[XML_LAYOUT] = LayoutManager::UIParseLayoutTagCallback;
  m_resource_node_parser_map[XML_LAYOUTCONFIG] =
      LayoutManager::UIParseLayoutConfigTagCallback;
  m_resource_node_parser_map[XML_INCLUDE] =
      SkinParseEngine::UIParseIncludeTagCallback;
  m_resource_node_parser_map[XML_I18N] = I18nManager::UIParseI18nTagCallback;

  m_render_base_factory.Init();
  m_text_render_base_factory.Init();
  m_layout_factory.Init();
}

// 用于编辑器中调整控件库的依赖
void Application::RestoreRegisterUIObject() {
  ClearRegisterUIObject();
  RegisterDefaultUIObject();
}

IObject *Application::CreateUIObjectByName(const char *name,
                                           IResourceBundle *resource_bundle) {
  if (!name)
    return nullptr;

  int nSize = (int)m_object_meta_array.size();
  for (int i = 0; i < nSize; i++) {
    if (0 == strcmp(name, m_object_meta_array[i]->Name())) {
      IObject *p = nullptr;
      m_object_meta_array[i]->Create(resource_bundle, (void **)&p);
      return p;
    }
  }

  UI_LOG_ERROR("CreateUIObjectByName failed, name=%s", name);
  return nullptr;
}

IObject *Application::CreateUIObjectByUUID(const Uuid &clsid,
                                            IResourceBundle *resource_bundle) {
  int nSize = (int)m_object_meta_array.size();
  for (int i = 0; i < nSize; i++) {
    if (clsid == m_object_meta_array[i]->UUID()) {
      IObject *p = nullptr;
      m_object_meta_array[i]->Create(resource_bundle, (void **)&p);
      return p;
    }
  }

  UI_LOG_ERROR("CreateUIObjectByUUID Failed.");
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

bool Application::IsHardwareCompositeEnable() { 
  return ui::GetGpuStartupState() == GPU_STARTUP_STATE::STARTED; 
}
bool Application::EnableHardwareComposite() {
  if (IsHardwareCompositeEnable())
    return true;

#if 1
  ui::GpuStartup();
#else
  ui::Slot<void()> func([]() {
    if (!ui::GpuStartup()) {
      UI_LOG_ERROR("GpuStartup Failed");
    }
  });
  
  RenderThread::Main::PostTask(std::move(func));
  // wait
  while (ui::GetGpuStartupState() <= GPU_STARTUP_STATE::STARTING) {
#if defined (OS_WIN)
    ::Sleep(10);
#else
    ::sleep(1);
#endif
  }
#endif
  return true;
}

void Application::ShutdownGpuCompositor() {
  if (!IsHardwareCompositeEnable())
    return;
#if 1
  ui::GpuShutdown();
#else
  ui::Slot<void()> func(&ui::GpuShutdown);
  RenderThread::Main::PostTask(std::move(func));
#endif
}

void Application::LoadUIObjectListToToolBox() {
  if (!m_pUIEditor)
    return;

  auto iter = m_object_meta_array.begin();
  for (; iter != m_object_meta_array.end(); iter++) {
    m_pUIEditor->OnToolBox_AddObject((*iter));
  }
}

std::shared_ptr<IRenderBase>
Application::CreateRenderBaseByName(IResourceBundle *resource, const char *name) {
  return m_render_base_factory.CreateRenderBaseByName(resource, name);
}

const char *Application::GetRenderBaseName(int nType) {
  return m_render_base_factory.GetRenderBaseName(nType);
}

} // namespace ui
