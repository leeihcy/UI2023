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

  RECT rc;
  ::GetClientRect(m_hWnd, &rc);
  RECT2Rect(rc, prect);
}

void WindowPlatformWin::GetWindowRect(Rect *prect) {
  if (!prect) {
    return;
  }

  RECT rc;
  ::GetWindowRect(m_hWnd, &rc);
  RECT2Rect(rc, prect);
}

void WindowPlatformWin::CenterWindow() {
  ::CenterWindow(m_hWnd);
}


void WindowPlatformWin::Submit(sk_sp<SkSurface> sksurface) {
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

void WindowPlatformWin::RECT2Rect(RECT& r, Rect* prect)
{
  prect->x = r.left;
  prect->y = r.top;
  prect->width = r.right-r.left;
  prect->height = r.bottom-r.top;
}

} // namespace ui