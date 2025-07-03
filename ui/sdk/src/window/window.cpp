#include "window.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/layout/desktop_layout.h"
#include "src/resource/layoutmanager.h"
#include "src/resource/res_bundle.h"
#include "src/window/linux/display_wayland.h"
#include "window_meta.h"

#include <SkColorSpace.h>
#include <assert.h>

#if defined(OS_WIN)
#include "window_win.h"
#elif defined(OS_MAC)
#include "window_mac.h"
#elif defined(OS_LINUX)
#include "window_linux_x11.h"
#include "window_linux_wayland.h"
#endif

namespace ui {

Window::Window(IWindow *p)
    : Panel(p), m_window_render(*this), m_mouse_key(*this), m_pIWindow(p) {
  memset(&m_window_style, 0, sizeof(m_window_style));
  UI_LOG_DEBUG("Window");
}
Window::~Window() {
  if (m_platform) {
    m_platform->Release();
  }
  UI_LOG_DEBUG("~Window");
}

void Window::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    Panel::onRouteMessage(msg);
    FinalConstruct();
    return;
  }
  if (msg->message == UI_MSG_ERASEBKGND) {
    Panel::onRouteMessage(msg);
    onEraseBkgnd(static_cast<EraseBkgndMessage *>(msg)->rt);
    return;
  }
  if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto *m = static_cast<QueryInterfaceMessage *>(msg);
    if (m->uuid == WindowMeta::Get().UUID()) {
      *(m->pp) = m_pIWindow;
      return;
    }
  }
  if (msg->message == UI_MSG_SERIALIZE) {
    auto *m = static_cast<SerializeMessage *>(msg);
    onSerialize(m->param);
    return;
  }
  Panel::onRouteMessage(msg);
}

long Window::FinalConstruct() {
#if 0
    this->m_oMouseManager.SetUIApplication(p->GetUIApplication()->GetImpl());
    this->m_oDragDropManager.SetWindowBase(this);
#endif
  m_window_style.hard_composite = GetUIApplication()->IsGpuCompositeEnable();

  return 0;
}

void Window::onSerialize(SerializeParam *pData) {
  // 放在最前面，设置好Graphics Render Library
  m_window_render.OnSerialize(pData);

  Panel::onSerialize(pData);
  AttributeSerializer s(pData, "Window");
#if 0
  s.AddString(XML_FONT, this,
              memfun_cast<pfnStringSetter>(&Window::SetDefaultRenderFont),
              memfun_cast<pfnStringGetter>(&Window::GetDefaultRenderFontId))
      ->SetDefault(nullptr);
#endif
  s.AddString(XML_TEXT, m_strConfigWindowText);
}

void Window::Create(const char *szId, const Rect* rect) {
#if defined(OS_WIN)
  m_platform.reset(new WindowPlatformWin(*this));
#elif defined(OS_MAC)
  m_platform.reset(new WindowPlatformMac(*this));
#elif defined(OS_LINUX)
  if (WaylandDisplay::IsWaylandDesktopEnviroment()) {
    m_platform.reset(new WindowPlatformLinuxWayland(*this));
  } else {
    m_platform.reset(new WindowPlatformLinuxX11(*this));
  }
#else
  assert(false);
#endif

  if (rect) {
    m_window_style.setcreaterect = true;
  }

  CreateUI(szId);
  
  m_platform->Initialize();

  Rect rc = {0, 0, 400, 400};
  if (rect) {
    rc.CopyFrom(*rect);
  }
  m_platform->Create(rc);

  onCreate();

  m_objLayer.CreateLayer();
}

WINDOW_HANDLE Window::GetWindowHandle() {
  return m_platform->GetWindowHandle();
}

void Window::SetTitle(const char *title_utf8) {
  m_platform->SetTitle(title_utf8);
}

void Window::Show() {
  if (m_platform) {
    m_platform->Show();
  }
}

void Window::enterResize(bool b) { m_window_style.enter_resize = b; }

// width height是乘以了缩放系统的值。
void Window::onSize(int width, int height) {
  if (m_width == width && m_height == height) {
    return;
  }
  m_width = width;
  m_height = height;

  // if (SIZE_MINIMIZED == wParam)
  //       return 0;

  Rect rcWindow;
  Rect rcParent;
  m_platform->GetWindowRect(&rcWindow);
  m_platform->GetClientRect(&rcParent);

  // UI_LOG_DEBUG(L"Window Size parent={%d,%d, %d,%d}, window={%d,%d, %d,%d}",
  //              rcParent.x, rcParent.y, rcParent.width, rcParent.height,
  //              rcWindow.x, rcWindow.y, rcWindow.width, rcWindow.height);

  m_rcParent.CopyFrom(rcParent);

  // if (GetConfigWidth() > 0)
  //   SetConfigWidth(rcWindow.width());
  // if (GetConfigHeight() > 0)
  //   SetConfigHeight(rcWindow.height());

  // 注：这里不要调用InvalidateRect。对于CustomWindow窗口拉大时，当前窗口的RGN还是老的设置，
  //     导致接收到WM_PAINT时的ps.rcPaint也被该老RGN剪裁，因此刷新区域不完整。这
  //     也导致接下来的CustomWindow::UpdateWindowRgn拿到的窗口背景不完整，计算剪裁
  //     区域错误。
  // 因此这里还是直接调用了窗口的UpdateObject
  //::InvalidateRect(m_hWnd, nullptr, FALSE);

  // if (m_window_style.enter_resize) {
  //   printf("m_window_style.enter_resize\n");
  // } else
  {
    notify_WM_SIZE(0, rcParent.width(), rcParent.height());
    // size_changed.emit((long)wParam);
    m_window_render.OnWindowSize(width, height);

    if (m_window_render.CanCommit()) {
      // 如果!cancommit，有可能是窗口刚创建时的初始化，直接走WM_PAINT消息
      // 然后由windowrender解除cancommit限制

      // this->Invalidate();
      // 需要立即刷新.  场景：
      // 窗口作为一个ws_child嵌入在其它窗口下面。当改变父窗口大小时，本窗口也响应
      // size改变，如果只延迟刷新，会导致自己的缓存被清空，但父窗口刷新时子窗口也要刷新，
      // 最后导致将自己的空缓存提交上去了，然后再延时刷新，界面又正常。
      // 因此这里不能延时刷新
      // TODO:if (m_objStyle.initialized && IsWindowVisible()) {
      // m_window_render.InvalidateNow();
      // TODO:}
      //  m_platform->ValidateRect(nullptr);
    } else {
      m_platform->Invalidate(nullptr);
    }
  }
}

void Window::onClose() {}
void Window::onDestroy() {
  WindowDestroyEvent event;
  event.window = m_pIWindow;
  emit(WINDOW_DESTROY_EVENT, &event);

  // 释放资源
  DestroyChildObject();
  // events中slot可能绑定了一些参数对象，需要释放掉。
  clear_events();
}

void Window::onPaint(Rect *dirty) {
  // if (dirty) {
  //   UI_LOG_DEBUG(L"Window onPaint {%d,%d, %d,%d}", dirty->x, dirty->y,
  //                dirty->width, dirty->height);
  // } else {
  //   UI_LOG_DEBUG(L"Window onPaint full");
  // }

  // if (dirty) {
  //   Rect rc;
  //   rc.CopyFrom(*dirty);
  //   m_objLayer.GetLayer()->Invalidate(&rc);
  // } else {
  //   m_objLayer.GetLayer()->Invalidate(nullptr);
  // }
  if (dirty) {
    m_window_render.OnWindowPaint(*dirty);
  } else {
    Rect rc = {0, 0, m_rcParent.width(), m_rcParent.height()};
    m_window_render.OnWindowPaint(rc);
  }

  // m_window_render.InvalidateNow();
}

void Window::Invalidate(const Rect *prc) { m_platform->Invalidate(prc); }

// void Window::on_paint(SkCanvas &canvas) { m_signal_paint.emit(canvas); }
// void Window::on_erase_bkgnd(SkCanvas &canvas) {}
// void Window::swap_buffer() { m_platform->Submit(m_sksurface); }

bool Window::CreateUI(const char *szId) {
  if (!m_pSkinRes) {
    UI_LOG_ERROR(TEXT("未初始化皮肤"));
    return false;
  }

  if (szId && strlen(szId) > 0) {
    LayoutManager &layoutmanager = m_pSkinRes->GetLayoutManager();
    //	加载子控件
    const char *szName = "";
    if (GetMeta())
      szName = GetMeta()->Name();

    UIElementProxy pUIElement = layoutmanager.FindWindowElement(szName, szId);
    if (pUIElement) {
      // 自己的属性
      this->LoadAttributeFromXml(pUIElement.get(), false);

      // 遍历子对象
      layoutmanager.ParseChildElement(pUIElement.get(), this);
    } else {
      UI_LOG_FATAL("获取窗口对应的xml结点失败：name=%s, id=%s", szName, szId);

      return false;
    }

    m_strId = szId; // 提前给id赋值，便于日志输出
  } else {
    InitDefaultAttrib();
  }
  //
  //	为了解决xp、win7上面的一个特性：只有在按了ALT键，或者TAB键之后，才会显示控件的focus
  // rect 	在初始化后，主动将该特性清除。
  //
  // ::PostMessage(m_hWnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR,
  // UISF_HIDEACCEL|UISF_HIDEFOCUS), 0);

  // 为了实现列表框，树控件的theme绘制，否则画出来的效果不正确，就是一个黑边
  // http://msdn.microsoft.com/zh-cn/library/windows/desktop/bb759827(v=vs.85).aspx
  // http://www.codeproject.com/Articles/18858/Fully-themed-Windows-Vista-Controls
  // http://stackoverflow.com/questions/14039984/which-class-part-and-state-is-used-to-draw-selection

  // The following example code gives a list-view control the appearance of a
  // Windows Explorer list: SetWindowTheme(m_hWnd, L"explorer", nullptr);

  // 窗口作为根结点，一定会创建一个缓存
  m_objStyle.layer = 1;

  return true;
}

void Window::onCreate() {

#if 0
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
#endif

  // 布局
  if (m_window_style.attach) // attach的窗口直接使用外部的大小
  {
    // ::GetClientRect(m_hWnd, &m_rcParent);

    //     // 避免此时调用GetDesiredSize又去测量窗口大小了，
    //     // 导致窗口被修改为自适应大小
    // CRect rcWindow;
    // ::GetWindowRect(m_hWnd, &rcWindow);
    // SetConfigWidth(rcWindow.Width());
    // SetConfigHeight(rcWindow.Height());

    //     // 因为Attach到的窗口初始化时已经收不到WM_SIZE了，
    //     // 因此自己再发一次，
    //     // 通知创建RenderTarget，否则后面的一些刷新将失败
    // notify_WM_SIZE(0, m_rcParent.Width(), m_rcParent.Height());
    //     this->UpdateLayout();
  } else {
    if (m_window_style.setcreaterect) {
      // 避免此时调用GetDesiredSize又去测量窗口大小了，
      // 导致窗口被修改为自适应大小
      Rect rcWindow;
      m_platform->GetWindowRect(&rcWindow);
      SetConfigWidth(rcWindow.Width());
      SetConfigHeight(rcWindow.Height());

      m_platform->GetClientRect(&m_rcParent);
      this->UpdateLayout();
    } else {
      // 不能放在 OnInitialize 后面。
      // 因为有可能OnInitialize中已经调用过 SetWindowPos
      DesktopLayout dl;
      dl.Arrange(this);
    }
  }

#if 0
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
  if (m_pCallbackProxy) {
    m_pCallbackProxy->DoBindPlz(true);
  }
  if (m_pCallbackProxy) {
    m_pCallbackProxy->OnWindowInit();
  }

  m_oWindowRender.SetCanCommit(true);

  // 设置默认对象
  m_oMouseManager.SetDefaultObject(m_oMouseManager.GetOriginDefaultObject(),
                                   false);

  if (m_window_style.attach) // 主动触发刷新
  {
    Invalidate();
  }

  IUIAutoTest *pAutoTest = GetUIApplication()->GetUIAutoTestPtr();
  if (pAutoTest) {
    pAutoTest->OnWindowInit(static_cast<IWindow *>(m_pIWindowBase));
  }
#endif
}

void Window::SetGpuComposite(bool b) {
#if 0 // defined(OS_WIN)
  UIASSERT(!m_hWnd && TEXT("该函数目前需要在窗口创建之前调用"));
#endif

  if (b) {
    if (!GetUIApplication()->IsGpuCompositeEnable())
      b = false;
  }

  if (b == m_window_style.hard_composite)
    return;

  m_window_style.hard_composite = b;
  // m_oWindowRender.OnHardCompositeChanged(b);

  // UI_LOG_DEBUG(TEXT("hard composite enable, window=0x%08x"), this);
}

void Window::onEraseBkgnd(IRenderTarget *pRenderTarget) {
  if (nullptr == pRenderTarget)
    return;

  WindowPaintEvent event;
  event.window = m_pIWindow;
  event.rt = pRenderTarget;
  emit(WINDOW_PAINT_EVENT, &event);

  // SetMsgHandled(false);
}

#if 0
// long WindowBase::_OnPostMessage( unsigned int uMsg, long wParam, long lParam, BOOL& bHandled )
// {
// 	bHandled = FALSE;
//   if (wParam == MSG_ASYNCTASK)
// 	{
// 		std::function<void(WindowBase*)>* callback = 
// 			(std::function<void(WindowBase*)>*)lParam;

// 		(*callback)(this);

// 		delete callback;
// 	}
// 	return 0;
// }

// void  WindowBase::AsyncTask(std::function<void(WindowBase*)> callback)
// {
// 	if (!callback)
// 		return;

// 	std::function<void(WindowBase*)>* param =
// 		new std::function<void(WindowBase*)>(callback);

// 	::PostMessage(m_hWnd, UI_MSG_POSTMESSAGE, MSG_ASYNCTASK, (long)param);
// }
#endif

bool Window::IsGpuComposite() { return m_window_style.hard_composite; }
void Window::DirectComposite() { assert(0 && "这个函数是否还需要继续存在?"); }

bool Window::IsChildWindow() { return m_platform->IsChildWindow(); }

bool Window::IsWindowVisible() { return m_platform->IsWindowVisible(); }

void Window::Commit(IRenderTarget *pRT, const Rect *prect, int count) {
  m_platform->Commit(pRT, prect, count);
}

float Window::GetScaleFactor() { return m_platform->GetScaleFactor(); }


void  Window::SetObjectPos( int x, int y, int cx, int cy, SetPositionFlags flags)
{
  m_platform->SetWindowPos(x, y, cx, cy, flags);
  m_platform->GetClientRect(&m_rcParent);
}

} // namespace ui