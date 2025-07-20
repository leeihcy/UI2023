#include "window.h"
#include "include/interface/iattribute.h"
#include "include/interface/imessage.h"
#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "include/util/rect.h"
#include "src/application/uiapplication.h"
#include "src/layout/desktop_layout.h"
#include "src/panel/root_object.h"
#include "src/panel/panel.h"
#include "src/resource/layoutmanager.h"
#include "src/resource/res_bundle.h"
#include "window_meta.h"

#include <SkColorSpace.h>
#include <assert.h>

#if defined(OS_WIN)
#include "window_win.h"
#elif defined(OS_MAC)
#include "window_mac.h"
#elif defined(OS_LINUX)
#include "src/window/linux/display_wayland.h"
#include "window_linux_wayland.h"
#include "window_linux_x11.h"
#endif

namespace ui {

Window::Window(IWindow *p)
    : Message(p), m_window_render(*this), m_mouse_key(*this), m_pIWindow(p),
      m_root(nullptr, nullptr) {
  memset(&m_window_style, 0, sizeof(m_window_style));
  UI_LOG_DEBUG("Window");
}
Window::~Window() {
  if (m_platform) {
    m_platform->Release();
  }
  UI_LOG_DEBUG("~Window");
}

RootObject &Window::GetRootObject() { return *m_root->GetImpl(); }

void Window::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    Message::onRouteMessage(msg);
    FinalConstruct(static_cast<FinalConstructMessage *>(msg));
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
  // if (msg->message == UI_MSG_GETDESIREDSIZE) {
  //   onGetDesiredSize(&static_cast<GetDesiredSizeMessage *>(msg)->size);
  //   return;
  // }
  Message::onRouteMessage(msg);
  m_root->onRouteMessage(msg);
}

void Window::FinalConstruct(FinalConstructMessage *message) {
  if (!message->resource) {
    message->success = false;
    return;
  }
  m_resource = message->resource->GetImpl();

#if 0
    this->m_oMouseManager.SetUIApplication(p->GetUIApplication()->GetImpl());
    this->m_oDragDropManager.SetWindowBase(this);
#endif

  m_window_style.hard_composite =
      m_resource->GetUIApplication()->IsGpuCompositeEnable();

  m_root = IRootObject::create(message->resource);
  if (!m_root) {
    message->success = false;
    return;
  }
  GetRootObject().BindWindow(this);
}

void Window::Create(const char *layout_id, const Rect *rect) {
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
  m_platform->Initialize();

  m_window_render.SetCanCommit(false);

  PreCreateWindowMessage message;
  if (rect) {
    // 外部指定了窗口大小
    message.param.position = true;
    message.param.x = rect->left;
    message.param.y = rect->top;
    message.param.w = rect->Width();
    message.param.h = rect->Height();
  }
  RouteMessage(&message);

  GetRootObject().LoadLayout(layout_id);
  m_platform->Create(message.param);

  postCreate(message.param);
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
void Window::onSize(int window_width, int window_height) {
  if (m_window_width == window_width && m_window_height == window_height) {
    return;
  }
  m_window_width = window_width;
  m_window_height = window_height;

  Rect rc_client;
  m_platform->GetClientRect(&rc_client);

  Rect rc_client_old;
  GetRootObject().GetParentRect(&rc_client_old);

  if (rc_client.Width() == rc_client_old.Width() &&
      rc_client.Height() == rc_client_old.Height()) {
    return;
  }

  int old_client_width = rc_client_old.Width();
  int old_client_height = rc_client_old.Height();
  int new_client_width = rc_client.Width();
  int new_client_height = rc_client.Height();

  // 计算无效区域
  ui::Rect rc_invalid_right = ui::Rect::MakeLTRB(
      old_client_width, 0, new_client_width, new_client_height);
  ui::Rect rc_invalid_bottom = ui::Rect::MakeLTRB(
      0, old_client_height, old_client_width, new_client_height);

  // m_window_render.AddInvalidateRect(&rc_invalid_right);
  // m_window_render.AddInvalidateRect(&rc_invalid_bottom);
  m_window_render.AddInvalidateRect(&rc_client);

  GetRootObject().OnWindowSize(new_client_width, new_client_height);
  m_window_render.OnWindowSize(new_client_width, new_client_height);

  // TBD:
  // m_window_render.Paint();

#if 0
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
      m_window_render.RequestUpdate();
      // TODO:}
      //  m_platform->ValidateRect(nullptr);
    } else {
      // m_platform->Invalidate(nullptr);
    }
  }
#endif
}

void Window::onClose() {}
void Window::onDestroy() {
  WindowDestroyEvent event;
  event.window = m_pIWindow;
  emit(WINDOW_DESTROY_EVENT, &event);

  // 释放资源
  m_root->GetImpl()->DestroyChildObject();
  // events中slot可能绑定了一些参数对象，需要释放掉。
  clear_events();
}

void Window::onPaint(const Rect *commit_rect) {
  // if (dirty) {
  //   UI_LOG_DEBUG(L"Window onPaint {%d,%d, %d,%d}", dirty->x, dirty->y,
  //                dirty->width, dirty->height);
  // } else {
  //   UI_LOG_DEBUG(L"Window onPaint full");
  // }

  // m_window_render.AddInvalidateRect(dirty);
  m_window_render.Paint(commit_rect);
  m_window_render.Commit(commit_rect);

  // if (dirty) {
  //   m_window_render.OnWindowPaint(*dirty);
  // } else {
  //   Rect rc = {0, 0, m_rcParent.width(), m_rcParent.height()};
  //   m_window_render.OnWindowPaint(rc);
  // }

  // m_window_render.InvalidateNow();
}

// void Window::on_paint(SkCanvas &canvas) { m_signal_paint.emit(canvas); }
// void Window::on_erase_bkgnd(SkCanvas &canvas) {}
// void Window::swap_buffer() { m_platform->Submit(m_sksurface); }

void Window::postCreate(CreateWindowParam &param) {

  m_window_render.OnWindowCreate();

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
    if (param.position) {
      // 避免此时调用GetDesiredSize又去测量窗口大小了，
      // 导致窗口被修改为自适应大小
      auto &root = GetRootObject();
      root.syncWindowSize();
      root.UpdateLayout();
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
#endif
  // 防止在实现显示动画时，先显示了一些初始化中刷新的内容。
  // 注：不能只限制一个layer
  
  {
    m_window_style.initialized = 1;

    GetRootObject().onWindowCreate();
    m_platform->PostCreate();
  }
#if 0
  if (m_pCallbackProxy) {
    m_pCallbackProxy->DoBindPlz(true);
  }
  if (m_pCallbackProxy) {
    m_pCallbackProxy->OnWindowInit();
  }
#endif

  m_window_render.SetCanCommit(true);

  // 将窗口所有区域设置为无效
  ui::Rect rc_client;
  m_platform->GetClientRect(&rc_client);
  m_window_render.AddInvalidateRect(&rc_client);

#if 0
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

  auto *app = m_resource->GetUIApplication();
  if (b) {
    if (!app->IsGpuCompositeEnable())
      b = false;
  }

  if (b == m_window_style.hard_composite)
    return;

  m_window_style.hard_composite = b;
  // m_oWindowRender.OnHardCompositeChanged(b);

  // UI_LOG_DEBUG(TEXT("hard composite enable, window=0x%08x"), this);
}


// 为了解决在<window>结点上配置 width/height
// 属性后，窗口实际大小存在歧义的问题： . 将 width/height 作为窗口整体大小； .
// 将 width/height 作为客户区域大小，窗口大小还需要再加上边框和标题栏尺寸；
// 现做如下规定：
// 1.
// <window>的width/height属性，如同layout.left等，都是做为窗口整体的布局属性，
//    不是指客户区域的大小。
// 2. 如果需要指定客户区的大小，需要创建一个root panel，设置它的width/height，
//    窗口最终大小将动态计算，最终加上边框和标题栏的范围。
// 3. Window响应GetDesiredSize消息，填充non client rect。
Size Window::GetDesiredSize() {
  Size size = GetRootObject().GetDesiredSize();
  ui::Rect rc_client = ui::Rect::MakeXYWH(0, 0, size.width, size.height);
  m_platform->UpdateNonClientRegion(&rc_client);
  return Size{rc_client.Width(), rc_client.Height()};
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

void Window::SetWindowPos(int x, int y, int cx, int cy,
                          SetPositionFlags flags) {
  // m_platform->SetWindowPos(x, y, cx, cy, flags);
  // m_platform->GetClientRect(&m_rcParent);
}

} // namespace ui