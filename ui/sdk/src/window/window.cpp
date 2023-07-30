#include "window.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/resource/layoutmanager.h"
#include "src/resource/res_bundle.h"

#include <SkColorSpace.h>
#include <assert.h>

#if defined(OS_MAC)
#include "window_mac.h"
#elif defined(OS_LINUX)
#include "window_linux.h"
#endif

namespace ui {

Window::Window(IWindow *p) : Object(p), m_window_render(*this), m_pIWindow(p) {}
Window::~Window() {
  if (m_platform) {
    m_platform->Release();
    delete m_platform;
    m_platform = nullptr;
  }
}

long Window::FinalConstruct(IResBundle *p) {
  DO_PARENT_PROCESS(IWindow, /*IPanel*/ IObject);

#if 0
    this->m_oMouseManager.SetUIApplication(p->GetUIApplication()->GetImpl());
    this->m_oDragDropManager.SetWindowBase(this);
#endif
  m_windowStyle.hard_composite =
      false; // p->GetUIApplication()->IsGpuCompositeEnable();

  return 0;
}

void Window::OnSerialize(SERIALIZEDATA *pData) {
  // 放在最前面，设置好Graphics Render Library
  m_window_render.OnSerialize(pData);
  // Panel::OnSerialize(pData);
  Object::OnSerialize(pData);

  AttributeSerializer s(pData, TEXT("Window"));
#if 0
  s.AddString(XML_FONT, this,
              memfun_cast<pfnStringSetter>(&Window::SetDefaultRenderFont),
              memfun_cast<pfnStringGetter>(&Window::GetDefaultRenderFontId))
      ->SetDefault(nullptr);
#endif
  s.AddString(XML_TEXT, m_strConfigWindowText);
}

void Window::Create(const Rect &rect) {
#if defined(OS_WIN)
#elif defined(OS_MAC)
  m_platform = new WindowPlatformMac(*this);
#elif defined(OS_LINUX)
  m_platform = new WindowPlatformLinux(*this);
#else
  assert(false);
#endif

  CreateUI(nullptr);

  m_platform->Initialize();
  m_platform->Create(rect);
}

void Window::SetTitle(const char *title) { m_platform->SetTitle(title); }

void Window::Show() {
  if (m_platform) {
    m_platform->Show();
  }
}

void Window::onSize(int width, int height) {
  if (m_width == width && m_height == height) {
    return;
  }
  m_width = width;
  m_height = height;
  // printf("on size: %d %d\n", width, height);
  // SkImageInfo info = SkImageInfo::Make(width, height, kBGRA_8888_SkColorType,
  //                                      kPremul_SkAlphaType, nullptr);
  // SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);
  // m_sksurface = SkSurface::MakeRaster(info, &surfaceProps);

  // if (SIZE_MINIMIZED == wParam)
  //       return 0;

  Rect rcWindow;
  Rect rcParent;
  m_platform->GetWindowRect(&rcWindow);
  m_platform->GetClientRect(&rcParent);

  UI_LOG_DEBUG(L"Window Size parent={%d,%d, %d,%d}, window={%d,%d, %d,%d}",
               rcParent.x, rcParent.y, rcParent.width, rcParent.height,
               rcWindow.x, rcWindow.y, rcWindow.width, rcWindow.height);

  m_rcParent.CopyFrom(rcParent);

  if (GetConfigWidth() > 0)
    SetConfigWidth(rcWindow.width);
  if (GetConfigHeight() > 0)
    SetConfigHeight(rcWindow.height);

  notify_WM_SIZE(0, rcParent.width, rcParent.height);
#if 0
  size_changed.emit((long)wParam);
#endif

  m_window_render.OnWindowSize(width, height);

  // 注：这里不要调用InvalidateRect。对于CustomWindow窗口拉大时，当前窗口的RGN还是老的设置，
  //     导致接收到WM_PAINT时的ps.rcPaint也被该老RGN剪裁，因此刷新区域不完整。这
  //     也导致接下来的CustomWindow::UpdateWindowRgn拿到的窗口背景不完整，计算剪裁
  //     区域错误。
  // 因此这里还是直接调用了窗口的UpdateObject
  //::InvalidateRect(m_hWnd, nullptr, FALSE);
  if (m_window_render.CanCommit()) {
    // 如果!cancommit，有可能是窗口刚创建时的初始化，直接走WM_PAINT消息
    // 然后由windowrender解除cancommit限制

    // this->Invalidate();
    // 需要立即刷新.  场景：
    // 窗口作为一个ws_child嵌入在其它窗口下面。当改变父窗口大小时，本窗口也响应
    // size改变，如果只延迟刷新，会导致自己的缓存被清空，但父窗口刷新时子窗口也要刷新，
    // 最后导致将自己的空缓存提交上去了，然后再延时刷新，界面又正常。
    // 因此这里不能延时刷新
    if (m_objStyle.initialized && IsWindowVisible()) {
      m_window_render.InvalidateNow();
    }
    m_platform->ValidateRect(nullptr);
  } else {
    m_platform->InvalidateRect(nullptr);
  }
}

void Window::onClose() {}
void Window::onDestroy() { m_signal_destroy.emit(); }

#if 0

// 该消息已在WM_PAINT中分发。
// 为了实现无闪烁的绘制，必须将所有的绘制代码放在一个地方处理，要么在WM_ERASEBKGND，要么在WM_PAINT
// 但WM_ERASEBKGND不是每次都会触发的（在处理异形窗口时出现该问题），因此考虑将绘制代码都放在WM_PAINT中处理
// 
// 如果 lParam == 0，则表示是系统自己发出来的消息，由于系统消息会导致和WM_PAINT DC不一致，从而产生闪烁
// 因此将WM_ERASEBKGND消息放在WM_PAINT中由我们自己发出，并且将lParam置为非0
long WindowBase::_OnEraseBkgnd( unsigned int uMsg, long wParam, long lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (0 == lParam)
	{
		bHandled = TRUE;
		if (m_bFirsetEraseBkgnd)
		{
			m_bFirsetEraseBkgnd = false;


            // 分层窗口不处理。场景：
            // 在窗口创建完成后，设置分层窗口的大小，此时在这里又触发了分层窗口刷新，导致
            // 窗口大小被修改为旧的大小，新的windowposchanged/onsize消息无效
            if (!(GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED))
            {
                m_oWindowRender.InvalidateNow();
            }

			// 如果什么也不做，会导致窗口第一次显示时，窗口先显示一次黑色，例如combobox.listbox/menu
			// 如果直接调用DefWindowProc会导致窗口显示白色，但最终显示的界面不一定就是白色的，也会导致闪烁
            // 因此在这里先做一次全量绘制
			// DefWindowProc(uMsg, wParam, lParam); 

            // Bug 2014.3.7 在分层窗口中，第一次触发该函数时，有这种情况:
            //   ShowWindow(SW_SHOW);
            //   SetWidowPos(x, y, cx, cy);
            // 结果在SetWindowPos中触发第一次erasebkgnd，但还没有走到windowposchanging中，导致
            // layeredwindow::commit中使用旧的窗口位置提交分层窗口，新的窗口位置无效（但分层窗口中
            // 保存的仍然是x,y,下一次刷新才能更新到正确位置)，因此将ValidateRect(nullptr)注掉，避免出现这种问题
            // 
		}
		
		return 1;   // 对于Dialog类型，需要返回1来阻止系统默认绘制
	}
	return 0;
}

//
// [注]分层窗口的InvalidateRect，拿到的ps.rcPaint永远是空的
//
long WindowBase::_OnPaint( unsigned int uMsg, long wParam,long lParam, BOOL& bHandled )
{
	PAINTSTRUCT ps;
	HDC  hDC = nullptr;
	RECT rcInvalid = {0};

	if (0 == wParam)
	{
		hDC = ::BeginPaint(this->m_hWnd ,&ps);
		rcInvalid = ps.rcPaint;

		if (IsRectEmpty(&rcInvalid) && 
			GetWindowLongPtr(m_hWnd, GWL_EXSTYLE)&WS_EX_LAYERED)
		{
			GetClientRect(m_hWnd, &rcInvalid);
		}
	}
	else
	{
		GetClipBox(hDC, &rcInvalid);
        if (IsRectEmpty(&rcInvalid))
        {
            GetClientRect(m_hWnd, &rcInvalid);
        }
	}

	if (!IsRectEmpty(&rcInvalid))
	{
// 		if (m_bFirstTimePaintWindow)
// 		{
// 			m_bFirstTimePaintWindow = false;
// 			m_oWindowRender.SetCanCommit(true);
// 		}

        // 2016.3.12 现在WM_PAINT一般都由操作系统触发，例如非透明主题下的刷新。
        // UISDK不建议再走WM_PAINT。因此这里再响应WM_PAINT时，直接提交缓存，不
        // 再刷新
        m_oWindowRender.OnPaint(hDC, &rcInvalid);
	}
	
	if(0 == wParam)
	{
		EndPaint(m_hWnd,&ps);
	}
	
	return 1;  //  在_OnPaint中返回0，会导致dialog类型的窗口，被其它窗口覆盖后移出来刷新异常!!!
}
#endif

void Window::onPaint(Rect *dirty) {
  if (dirty) {
    UI_LOG_DEBUG(L"Window onPaint {%d,%d, %d,%d}", dirty->x, dirty->y,
                 dirty->width, dirty->height);
  } else {
    UI_LOG_DEBUG(L"Window onPaint full");
  }

  // if (!m_sksurface) {
  //   return;
  // }

  // SkCanvas *canvas = m_sksurface->getCanvas();
  // on_erase_bkgnd(*canvas);
  // on_paint(*canvas);

  // m_sksurface->flushAndSubmit();
  // swap_buffer();

  // m_window_render.OnPaint(dirty);
  m_window_render.InvalidateNow();
}

void Window::on_paint(SkCanvas &canvas) { m_signal_paint.emit(canvas); }
void Window::on_erase_bkgnd(SkCanvas &canvas) {}
void Window::swap_buffer() { m_platform->Submit(m_sksurface); }

bool Window::CreateUI(const wchar_t *szId) {
  if (!m_pSkinRes) {
    UI_LOG_ERROR(TEXT("未初始化皮肤"));
    return false;
  }

  if (szId && wcslen(szId) > 0) {
    LayoutManager &layoutmanager = m_pSkinRes->GetLayoutManager();
    //	加载子控件
    const wchar_t *szName = L"";
    if (m_pDescription)
      szName = m_pDescription->GetTagName();

    UIElementProxy pUIElement = layoutmanager.FindWindowElement(szName, szId);
    if (pUIElement) {
      // 自己的属性
      this->LoadAttributeFromXml(pUIElement.get(), false);

      // 遍历子对象
      layoutmanager.ParseChildElement(pUIElement.get(), this);
    } else {
      UI_LOG_FATAL(_T("获取窗口对应的xml结点失败：name=%s, id=%s"), szName,
                   szId);

      return false;
    }

    m_strId = szId; // 提前给id赋值，便于日志输出
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
  m_objLayer.CreateLayer();
  return true;
}

void Window::SetGpuComposite(bool b) {
#if defined(OS_WIN)
  UIASSERT(!m_hWnd && TEXT("该函数目前需要在窗口创建之前调用"));
#endif

  if (b) {
    if (!GetUIApplication()->IsGpuCompositeEnable())
      b = false;
  }

  if (b == m_windowStyle.hard_composite)
    return;

  m_windowStyle.hard_composite = b;
  // m_oWindowRender.OnHardCompositeChanged(b);

  // UI_LOG_DEBUG(TEXT("hard composite enable, window=0x%08x"), this);
}

#if 0
long WindowBase::_OnPostMessage( unsigned int uMsg, long wParam, long lParam, BOOL& bHandled )
{
	bHandled = FALSE;
  if (wParam == MSG_ASYNCTASK)
	{
		std::function<void(WindowBase*)>* callback = 
			(std::function<void(WindowBase*)>*)lParam;

		(*callback)(this);

		delete callback;
	}
	return 0;
}

void  WindowBase::AsyncTask(std::function<void(WindowBase*)> callback)
{
	if (!callback)
		return;

	std::function<void(WindowBase*)>* param =
		new std::function<void(WindowBase*)>(callback);

	::PostMessage(m_hWnd, UI_MSG_POSTMESSAGE, MSG_ASYNCTASK, (long)param);
}
#endif

bool Window::IsGpuComposite() { return m_windowStyle.hard_composite; }
void Window::DirectComposite() { assert(0 && "这个函数是否还需要继续存在?"); }

bool Window::IsChildWindow() { return m_platform->IsChildWindow(); }

bool Window::IsWindowVisible() { return m_platform->IsWindowVisible(); }

} // namespace ui