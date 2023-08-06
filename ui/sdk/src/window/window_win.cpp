#include "window_linux.h"
#include <string.h>

namespace ui {

WindowPlatformWin::WindowPlatformWin(ui::Window &w) : m_ui_window(w) {}
void WindowPlatformWin::Initialize() {  }

void WindowPlatformWin::Release() {
  Destroy();
}

bool WindowPlatformWin::Create(const Rect &rect) {
  HWND hWnd = nullptr;
  UIASSERT(false);
  Attach(hWnd);
  return true;
}

bool WindowPlatformWin::CreateTransparent(const Rect &rect) {

  return true;
}

void WindowPlatformWin::Destroy() {
  if (!m_hWnd) {
    return;
  }
  ::DestroyWindow(m_hWnd);
}

void WindowPlatformWin::Attach(HWND hWnd) {
  if (!m_hWnd) {
    return;
  }

  m_hWnd = hWnd;
}

HWND WindowPlatformWin::Detach() {
  HWND hWnd = m_hWnd;
  m_hWnd = nullptr;
  return hWnd;
}

void WindowPlatformWin::Show() {
  ::ShowWindow(m_hWnd, SW_SHOW);
}

void WindowPlatformWin::Hide() { XUnmapWindow(m_display, m_window); }

void WindowPlatformWin::SetBorderless(bool no_border) {
}

void WindowPlatformWin::Invalidate() {
  ::InvalidateRect(m_hWnd, nullptr, TRUE);
}

std::string WindowPlatformWin::GetWindowTitle() {
  char text[256] = {0};
  ::GetWindowTextA(m_hWnd, text);
  return std::string(text);
}
void WindowPlatformWin::SetWindowTitle(const char *title) {
  ::SetWindowTextA(m_hWnd, title);
}

void WindowPlatformWin::SetMinMaxSize(int wMin, int hMin, int wMax,
                                        int hMax) {
UIASSERT(false);
}

/**
 * 1. 只有在被Map后，才能进行窗口Move操作，否则窗口管理器将无视这些设置。
 */
void WindowPlatformWin::SetWindowRect(int x, int y, int width, int height) {
  ::SetWindowPos(m_hWnd, 0, x, y, width, height, SWP_NOZORDER);
}

void WindowPlatformWin::GetClientRect(Rect *prect) {
  if (!prect) {
    return;
  }

  Rect rc;
  ::GetClientRect(m_hWnd, &rc);
  RECT2Rect(rc, prect);
}

void WindowPlatformWin::GetWindowRect(Rect *prect) {
  if (!prect) {
    return;
  }

  Rect rc;
  ::GetWindowRect(m_hWnd, &rc);
  RECT2Rect(rc, prect);
}

void WindowPlatformWin::CenterWindow() {
  ::CenterWindow(m_hWnd);
}


// void WindowPlatformWin::Submit(sk_sp<SkSurface> sksurface) {
void WindowPlatformWin::Submit(IRenderTarget* pRT, const Rect* prect, int count) {
  HDC hDC = GetDC(m_hWnd);
  for (uint i = 0; i < count; i++) {
    Rect &rcInWindow = prect[i];

    Render2TargetParam param = {0};
    param.xSrc = param.xDst = rcInWindow.left;
    param.ySrc = param.yDst = rcInWindow.top;
    param.wSrc = param.wDst = rcInWindow.right - rcInWindow.left;
    param.hSrc = param.hDst = rcInWindow.bottom - rcInWindow.top;
    m_pRootLayer->GetRenderTarget()->Render2DC(hDC, &param);
  }
  ReleaseDC(m_hWnd, hDC);
}

bool WindowPlatformWin::IsChildWindow()
{
	LONG_PTR lStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE );
	if( lStyle & WS_CHILD )
		return true;
	else
		return false;
}

bool  WindowPlatformWin::IsWindowVisible()
{
	if (!m_hWnd)
		return false;

	return ::IsWindowVisible(m_hWnd);
}

void WindowPlatformWin::RECT2Rect(Rect& r, Rect* prect)
{
  prect->x = r.left;
  prect->y = r.top;
  prect->width = r.right-r.left;
  prect->height = r.bottom-r.top;
}

#if 0
//
// [注]分层窗口的InvalidateRect，拿到的ps.rcPaint永远是空的
//
long WindowPlatformWin::_OnPaint( unsigned int uMsg, long wParam,long lParam, BOOL& bHandled )
{
	PAINTSTRUCT ps;
	HDC  hDC = nullptr;
	Rect rcInvalid = {0};

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
#endif

} // namespace ui