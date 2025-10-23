#include "window_win.h"
#include "include/util/math.h"
#include "include/util/rect.h"
#include <Windows.h>
#include <atlconv.h>

#include <ShellScalingApi.h> // 需要 Windows 8.1+ SDK
#include <winuser.h>
// #pragma comment(lib, "Shcore.lib") // 链接 Shcore.lib

void RECT2Rect(const RECT &r, ui::Rect *prect) {
  prect->left = r.left;
  prect->top = r.top;
  prect->right = r.right;
  prect->bottom = r.bottom;
}
void Rect2RECT(const ui::Rect &r, RECT *prect) {
  prect->left = r.left;
  prect->top = r.top;
  prect->right = r.right;
  prect->bottom = r.bottom;
}

namespace ui {

CREATE_WND_DATA WindowPlatformWin::s_create_wnd_data;
CREATE_WND_DATA::CREATE_WND_DATA() {
  m_pCreateWndList = nullptr;
  ::InitializeCriticalSection(&m_cs);
}
CREATE_WND_DATA::~CREATE_WND_DATA() { ::DeleteCriticalSection(&m_cs); }

void CREATE_WND_DATA::AddCreateWndData(_AtlCreateWndData *pData, void *pThis) {
  UIASSERT(pData != nullptr && pThis != nullptr);

  pData->m_pThis = pThis;
  pData->m_dwThreadID = ::GetCurrentThreadId();

  EnterCriticalSection(&m_cs);

  pData->m_pNext = this->m_pCreateWndList;
  this->m_pCreateWndList = pData;

  LeaveCriticalSection(&m_cs);
}
void *CREATE_WND_DATA::ExtractCreateWndData() {
  void *pv = nullptr;
  EnterCriticalSection(&m_cs);

  _AtlCreateWndData *pEntry = this->m_pCreateWndList;
  if (pEntry != nullptr) {
    DWORD dwThreadID = ::GetCurrentThreadId();
    _AtlCreateWndData *pPrev = nullptr;
    while (pEntry != nullptr) {
      if (pEntry->m_dwThreadID == dwThreadID) {
        if (pPrev == nullptr)
          this->m_pCreateWndList = pEntry->m_pNext;
        else
          pPrev->m_pNext = pEntry->m_pNext;
        pv = pEntry->m_pThis;
        break;
      }
      pPrev = pEntry;
      pEntry = pEntry->m_pNext;
    }
  }

  LeaveCriticalSection(&m_cs);
  return pv;
}

WindowPlatformWin::WindowPlatformWin(ui::Window &w) : m_ui_window(w) {}
WindowPlatformWin::~WindowPlatformWin() {}
void WindowPlatformWin::Initialize() {}

LRESULT CALLBACK StartWindowProc(HWND hwnd, unsigned int uMsg, WPARAM wParam,
                                 LPARAM lParam) {
  // 获取this指针
  WindowPlatformWin *pThis =
      (WindowPlatformWin *)
          WindowPlatformWin::s_create_wnd_data.ExtractCreateWndData();
  UIASSERT(nullptr != pThis);

  if (nullptr == pThis)
    return 0;

  return pThis->StartProc(hwnd, uMsg, wParam, lParam, true);
}

//
//	由StartWindowProc/StartDialogProc调用，将窗口过程转换为类对象的一个方法
//
LRESULT WindowPlatformWin::StartProc(HWND hwnd, unsigned int uMsg,
                                     WPARAM wParam, LPARAM lParam,
                                     bool bWindowOrDialog) {
  // 子类化
  set_hwnd(hwnd);

  this->m_thunk.Init((WNDPROC)&WindowPlatformWin::ThunkWndProc, this);
  WNDPROC pProc = this->m_thunk.GetWNDPROC();

  if (bWindowOrDialog) {
    this->m_oldWndProc = ::DefWindowProc;
    ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
  } else {
    this->m_oldWndProc = nullptr;
    ::SetWindowLongPtr(m_hWnd, DWLP_DLGPROC, (LONG_PTR)pProc);
  }

  // 调用新的窗口过程 ThunkWndProc
  return (LRESULT)pProc(hwnd, uMsg, wParam, lParam);
}

//
//	[static] long CALLBACK ThunkWndProc( HWND hwnd, unsigned int uMsg,
// WPARAM wParam, LPARAM lParam );
//
//	被ATL的thunk替换过的窗口过程
//
//	Parameter：
//		hwnd
//			[in]	这里由于被替换过了，这里的hwnd是this指针
//
//		uMsg,wParam,lParam
//			[in]	消息信息
//
LRESULT WindowPlatformWin::ThunkWndProc(HWND hwnd, unsigned int uMsg,
                                        WPARAM wParam, LPARAM lParam) {
  WindowPlatformWin *pThis = (WindowPlatformWin *)hwnd;
  LRESULT lRet = 0;

  if (!pThis->PreTranslateMessage(uMsg, wParam, lParam, &lRet))
    lRet = pThis->WndProc(uMsg, wParam, lParam);

  return (LRESULT)lRet;
}

bool WindowPlatformWin::PreTranslateMessage(unsigned int uMsg, WPARAM wParam,
                                            LPARAM lParam, LRESULT *pRet) {
  return false;
}

//
//	[private] long WndProc( unsigned int uMsg, WPARAM wParam, LPARAM lParam
//)
//
//	窗口被子类化过之后的窗口过程
//
LRESULT WindowPlatformWin::WndProc(unsigned int uMsg, WPARAM wParam,
                                   LPARAM lParam) {

  LRESULT lRes = 0;

  // // 内部处理
  BOOL bHandled = this->ProcessWindowMessage(
      m_hWnd, uMsg, wParam, lParam, lRes, 0); // 调用BEGIN_MSG_MAP消息映射列表
  if (bHandled) {
    return WndProc_GetRetValue(uMsg, wParam, lParam, bHandled, lRes);
  }

  // // 如果这个消息被处理过了，直接返回，不用再调用旧的窗口过程了
  // //if (static_cast<IMessage*>(this)->ProcessMessage(&msg, 0))
  //   UISendMessage(&msg);
  lRes = WndProc_GetRetValue(uMsg, wParam, lParam, bHandled, lRes);

  if (uMsg == WM_NCDESTROY) {
    // 注：为什么不在这里直接调用OnFinalMessage，却还要再加一个状态位？
    // 因为WM_NCDESTROY函数由DestroyWindow api触发，而DestroyWindow api
    // 可能位于任何一个当前窗口的消息响应中，因此当pOldMsg==nullptr时，即表示
    // 没有消息嵌套了，在检查一次WINDOW_STYLE_DESTROYED标志即可。
    m_ui_window.GetWindowStyle().destroyed = 1;
    // m_ui_window.m_objStyle.initialized = 0;
  }
  if (m_ui_window.GetWindowStyle().destroyed) {
    m_ui_window.GetWindowStyle().destroyed = 0;
  }

  return lRes;
}

// 设置对话框的DialogProc返回值，见MSDN中对DialogProc返回值的说明
LRESULT WindowPlatformWin::WndProc_GetRetValue(unsigned int uMsg, WPARAM wParam,
                                               LPARAM lParam, BOOL bHandled,
                                               LRESULT lRet) {
  LRESULT lResult = 0;

  if (nullptr == m_oldWndProc) // Dialog窗口过程
  {
    //   switch (uMsg) {
    //   case WM_INITDIALOG:
    //     if (GetMouseMgr()->GetFocusObject())
    //       lResult = FALSE; // 不使用其焦点设置
    //     else
    //       lResult = lRet;
    //     break;

    //   case WM_CHARTOITEM:
    //   case WM_COMPAREITEM:
    //   case WM_CTLCOLORBTN:
    //   case WM_CTLCOLORDLG:
    //   case WM_CTLCOLOREDIT:
    //   case WM_CTLCOLORLISTBOX:
    //   case WM_CTLCOLORSCROLLBAR:
    //   case WM_CTLCOLORSTATIC:
    //   case WM_CTLCOLORMSGBOX:
    //   case WM_QUERYDRAGICON:
    //   case WM_VKEYTOITEM:
    //     // return directly，这几个消息不按DialogProc返回值规定走
    //     lResult = lRet;
    //     break;

    //   default:
    //     SetWindowLongPtr(m_hWnd, DWLP_MSGRESULT, lRet);
    //     lResult = bHandled ? 1 : 0;
    //     break;
    //   }
  } else {
    if (bHandled) {
      lResult = lRet;
    } else {
      // lResult = DefWindowProc(uMsg, wParam, lParam);
      lResult = m_oldWndProc(m_hWnd, uMsg, wParam, lParam);
    }
  }
  return lResult;
}

void RegisterWndClass() {
  static bool registed = false;
  if (registed) {
    return;
  }
  registed = true;

  WNDCLASSEX wcex;

  // 注册UI普通窗口类
  wcex.cbSize = sizeof(WNDCLASSEX);

  // 分层窗口最大化时收不到redraw消息，因此直接在OnSize中刷新
  wcex.style = /*CS_HREDRAW | CS_VREDRAW |*/ CS_DBLCLKS;

  wcex.lpfnWndProc = (WNDPROC)StartWindowProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = 0; //::AfxGetInstanceHandle();;
  wcex.hIcon = 0;     // LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);

  // 2014.5.21
  // 将背景置为nullptr，解决aero磨砂窗口从最小化还原时，会先显示白色背景，
  // 再刷新内容的闪烁问题（触发了WM_NCCALCSIZE内部估计会做COPY操作）
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = WND_CLASS_NAME;
  wcex.hIconSm = 0; // LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
  RegisterClassEx(&wcex);

  // 注册用于实现动画的窗口类
  wcex.lpszClassName = WND_ANIMATE_CLASS_NAME;
  wcex.lpfnWndProc = ::DefWindowProc;
  RegisterClassEx(&wcex);

  // 注册用于实现菜单，弹出式列表框的窗口(带阴影/不带阴影)
  wcex.lpszClassName = WND_POPUP_CONTROL_SHADOW_NAME;
  wcex.style = CS_DROPSHADOW;
  wcex.lpfnWndProc = (WNDPROC)StartWindowProc;
  RegisterClassEx(&wcex);

  wcex.lpszClassName = WND_POPUP_CONTROL_NAME;
  wcex.style = 0;
  wcex.lpfnWndProc = (WNDPROC)StartWindowProc;
  RegisterClassEx(&wcex);

  // 拖拽时显示的图片窗口
  wcex.lpszClassName = WND_DRAGBITMAPWND_CLASS_NAME;
  wcex.style = 0;
  wcex.lpfnWndProc = ::DefWindowProc;
  RegisterClassEx(&wcex);

  // 由程序自己实现DPI缩放，不用系统帮忙。
  HMODULE hModule = GetModuleHandle(L"User32.dll");
  if (hModule) {
    // SetProcessDPIAware();
    FARPROC f = GetProcAddress(hModule, "SetProcessDPIAware");
    if (f)
      f();
  }
}

bool WindowPlatformWin::Create(CreateWindowParam &param) {
  RegisterWndClass();

  //	创建窗口句柄
  CREATESTRUCT cs;
  ::ZeroMemory(&cs, sizeof(CREATESTRUCT));
  long lStyle = 0;
  long lExStyle = 0;

  HWND hWndParent = 0;
  cs.hwndParent = hWndParent;
  if (lStyle)
    cs.style = lStyle;
  else
    cs.style = WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MINIMIZEBOX |
               WS_MAXIMIZEBOX | WS_CLIPCHILDREN;

  if (lExStyle)
    cs.dwExStyle = lExStyle;
  else
    cs.dwExStyle =
        WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;

  cs.lpszClass = WND_CLASS_NAME;
  cs.lpszName = _T("");

  // 获取DPI
  HMONITOR monitor = nullptr;
  if (param.position) {
    RECT rc = {param.x, param.y, param.x+param.w, param.y+param.h};
    monitor = ::MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
  } else {
    monitor = ::MonitorFromPoint({0,0}, MONITOR_DEFAULTTOPRIMARY);
  }
  UINT dpiX = 0, dpiY = 0;
  GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
  m_ui_window.m_dpi.SetSystemDpi(dpiX*1.0f/96);

  if (param.position) {
    Rect rc = ui::Rect::MakeXYWH(param.x, param.y, param.w, param.h);
    m_ui_window.m_dpi.ScaleRect(&rc);

    cs.x = rc.left;
    cs.y = rc.top;
    cs.cx = rc.Width();
    cs.cy = rc.Height();
    // m_ui_window.GetWindowStyle().setcreaterect = 1;
  } else {
    cs.x = cs.y = 0;
    // 这里不能直接写一个值。有可能窗口配置的也是这个大小，将导致收不到WM_SIZE消息，布局失败
    cs.cx = cs.cy = CW_USEDEFAULT; // 500;
  }

  s_create_wnd_data.AddCreateWndData(&m_thunk.cd, this);
  HWND hWnd =
      ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x,
                       cs.y, cs.cx, cs.cy, cs.hwndParent, 0, nullptr, nullptr);

  Attach(hWnd);

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

  return true;
}

bool WindowPlatformWin::CreateTransparent(const Rect &rect) { return true; }

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

WINDOW_HANDLE WindowPlatformWin::GetWindowHandle() {
  return (WINDOW_HANDLE)m_hWnd;
}

void WindowPlatformWin::Show(bool active) { 
  if (active) {
    ::ShowWindow(m_hWnd, SW_SHOW);
  } else {
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
  }
}

void WindowPlatformWin::Hide() { ::ShowWindow(m_hWnd, SW_HIDE); }

void WindowPlatformWin::SetBorderless(bool no_border) {}

// void WindowPlatformWin::Invalidate(const Rect *prect) {
//   ::InvalidateRect(m_hWnd, nullptr, TRUE);
// }

std::string WindowPlatformWin::GetTitle() {
  char text[256] = {0};
  ::GetWindowTextA(m_hWnd, text, 255);
  return std::string(text);
}
void WindowPlatformWin::SetTitle(const char *title_utf8) {
  ::SetWindowTextW(m_hWnd, ATL::CA2W(title_utf8, CP_UTF8));
}

void WindowPlatformWin::SetMinMaxSize(int wMin, int hMin, int wMax, int hMax) {
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

void WindowPlatformWin::UpdateNonClientRegion(Rect *pregion) {
  DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
  DWORD dwExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
  BOOL bMenu = FALSE;

  AdjustWindowRectEx((LPRECT)pregion, dwStyle, bMenu, dwExStyle);
}

extern "C" 
void GetWindowNormalRect(HWND hWnd, RECT *prc) {
  UIASSERT(hWnd);
  if (!hWnd)
    return;

  UIASSERT(prc);
  if (IsIconic(hWnd) || IsZoomed(hWnd)) {
    WINDOWPLACEMENT wndplacement = {0};
    wndplacement.length = sizeof(wndplacement);
    GetWindowPlacement(hWnd, &wndplacement);

    CopyRect((LPRECT)prc, &wndplacement.rcNormalPosition);
  } else {
    ::GetWindowRect(hWnd, (LPRECT)prc);
  }
}

void WindowPlatformWin::GetMonitorWorkArea(Rect *rect) {
  RECT rcParent = { 0 };
  RECT rcLimit = { 0 };

  // 计算出屏幕工作区域的大小(不使用整个屏幕的大小)
  // nCXScreen = ::GetSystemMetrics( SM_CXSCREEN );
  // nCYScreen = ::GetSystemMetrics( SM_CYSCREEN );

  if (GetWindowLong(m_hWnd, GWL_STYLE) & WS_CHILD) {
    HWND hWndParent = GetParent(m_hWnd);
    if (hWndParent) {
      ::GetClientRect(hWndParent, (LPRECT)&rcParent);
      rcLimit = rcParent;
    }
  } else {
    HWND hWndParent = GetParent(m_hWnd);
    if (hWndParent) {
      if (IsIconic(hWndParent)) {
        GetWindowNormalRect(hWndParent, &rcParent);
      } else {
        ::GetWindowRect(hWndParent, (LPRECT)&rcParent);
      }
      MONITORINFO mi = {sizeof(mi), 0};
      if (GetMonitorInfo(MonitorFromRect((LPRECT)&rcParent, MONITOR_DEFAULTTOPRIMARY),
                         &mi)) {
        rcLimit = mi.rcWork;
      } else {
        SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)&rcLimit, 0);
      }
    } else {
      POINT pt = {0};
      GetCursorPos(&pt);

      MONITORINFO mi = {sizeof(mi), 0};
      if (GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &mi)) {
        rcParent = mi.rcWork;
      } else {
        SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)&rcParent, 0);
      }
      rcLimit = rcParent;
    }
  }
  rect->Set(rcLimit.left, rcLimit.top, rcLimit.right, rcLimit.bottom);
}

void WindowPlatformWin::SetWindowPos(int x, int y, int w, int h,
                                     SetPositionFlags flags) {
  int windows_flags = 0;
  if (flags.move == false) {
    windows_flags |= SWP_NOMOVE;
  }
  if (flags.size == false) {
    windows_flags |= SWP_NOSIZE;
  }
  if (!flags.activate) {
    windows_flags |= SWP_NOACTIVATE;
  }
  ::SetWindowPos(m_hWnd, HWND_TOP, x, y, w, h, windows_flags);
}

void WindowPlatformWin::CenterWindow() {
  // ::CenterWindow(m_hWnd);
}

#if 0 // TODO:
void Render2DC(/*HDC*/ llong _hDC,
                                 Render2TargetParam *pParam) {
  if (!m_sksurface) {
    return;
  }
  HDC hDC = (HDC)_hDC;

  int &xDst = pParam->xDst;
  int &yDst = pParam->yDst;
  int &wDst = pParam->wDst;
  int &hDst = pParam->hDst;
  int &xSrc = pParam->xSrc;
  int &ySrc = pParam->ySrc;
  int &wSrc = pParam->wSrc;
  int &hSrc = pParam->hSrc;
  bool &bAlphaBlend = pParam->bAlphaBlend;
  byte &opacity = pParam->opacity;

  //  HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
  //  RECT rc = {xDst, yDst, xDst + wDst, yDst + hDst};
  // ::FillRect(hDC, &rc, hBrush);

  SkPixmap pm;
  if (!m_sksurface->peekPixels(&pm)) {
    return;
  }

  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(bmi));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = pm.width();
  bmi.bmiHeader.biHeight = -pm.height(); // top-down image
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;

  // if (wDst == wSrc && hDst == hSrc) {
  //   SetDIBitsToDevice
  // }
  ::StretchDIBits(hDC, xDst, ySrc, wDst, hDst, xSrc, ySrc, wSrc, hSrc,
                  pm.addr(), &bmi, DIB_RGB_COLORS, SRCCOPY);

  // if (bAlphaBlend) {
  //   BLENDFUNCTION bf = {AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA};
  //   ::AlphaBlend(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, wSrc, hSrc,
  //                bf);
  // } else {
  //   if (wDst == wSrc && hDst == hSrc) {
  //     ::BitBlt(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, SRCCOPY);
  //   } else {
  //     ::StretchBlt(hDstDC, xDst, ySrc, wDst, hDst, hDC, xSrc, ySrc, wSrc,
  //     hSrc,
  //                  SRCCOPY);
  //   }
  // }
}
#endif // TODO:

// 脏区域已转换成了像素坐标。
void WindowPlatformWin::Commit2(const FrameBuffer& fb, const RectRegion &dirty_region_px) {
  HDC hDC = GetDC(m_hWnd);
  for (unsigned int i = 0; i < dirty_region_px.Count(); i++) {
    Rect rcInWindow = dirty_region_px.RectPtr2()[i];

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = fb.width;
    bmi.bmiHeader.biHeight = -fb.height; // top-down image
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;

    ::StretchDIBits(hDC, rcInWindow.left, rcInWindow.top,
                    rcInWindow.right - rcInWindow.left,
                    rcInWindow.bottom - rcInWindow.top, rcInWindow.left,
                    rcInWindow.top, rcInWindow.right - rcInWindow.left,
                    rcInWindow.bottom - rcInWindow.top, fb.data, &bmi,
                    DIB_RGB_COLORS, SRCCOPY);
  }
  ReleaseDC(m_hWnd, hDC);
}

bool WindowPlatformWin::IsChildWindow() {
  LONG_PTR lStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
  if (lStyle & WS_CHILD)
    return true;
  else
    return false;
}

bool WindowPlatformWin::IsWindowVisible() {
  if (!m_hWnd)
    return false;

  return ::IsWindowVisible(m_hWnd);
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

// 该消息已在WM_PAINT中分发。
// 为了实现无闪烁的绘制，必须将所有的绘制代码放在一个地方处理，要么在WM_ERASEBKGND，要么在WM_PAINT
// 但WM_ERASEBKGND不是每次都会触发的（在处理异形窗口时出现该问题），因此考虑将绘制代码都放在WM_PAINT中处理
//
// 如果 lParam == 0，则表示是系统自己发出来的消息，由于系统消息会导致和WM_PAINT
// DC不一致，从而产生闪烁
// 因此将WM_ERASEBKGND消息放在WM_PAINT中由我们自己发出，并且将lParam置为非0
LRESULT WindowPlatformWin::_OnEraseBkgnd(UINT uMsg, WPARAM wParam,
                                         LPARAM lParam, BOOL &bHandled) {
  bHandled = TRUE;
  return 0;
}
//
// [注]分层窗口的InvalidateRect，拿到的ps.rcPaint永远是空的
//
LRESULT WindowPlatformWin::_OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    BOOL &bHandled) {
  PAINTSTRUCT ps;
  HDC hDC = nullptr;
  RECT rcInvalid = {0};

  if (0 == wParam) {
    hDC = ::BeginPaint(this->m_hWnd, &ps);
    // 注：这是物理像素坐标！
    rcInvalid = ps.rcPaint;

    if (IsRectEmpty(&rcInvalid) &&
        GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) {
      ::GetClientRect(m_hWnd, &rcInvalid);
    }
  } else {
    GetClipBox(hDC, &rcInvalid);
    if (IsRectEmpty(&rcInvalid)) {
      ::GetClientRect(m_hWnd, &rcInvalid);
    }
  }

  if (!IsRectEmpty(&rcInvalid)) {
    // m_oWindowRender.OnPaint(hDC, &rcInvalid);
    Rect dirty = {0, 0, 0, 0};
    RECT2Rect(rcInvalid, &dirty);
    m_ui_window.onPaint(&dirty);
  }

  if (0 == wParam) {
    EndPaint(m_hWnd, &ps);
  }

  return 1; //  在_OnPaint中返回0，会导致dialog类型的窗口，被其它窗口覆盖后移出来刷新异常!!!
}

LRESULT WindowPlatformWin::_OnDestroy(unsigned int uMsg, WPARAM wParam,
                                      LPARAM lParam, BOOL &bHandled) {
  bHandled = FALSE;
  return 0;
}

LRESULT WindowPlatformWin::_OnNcDestroy(unsigned int uMsg, WPARAM wParam,
                                        LPARAM lParam, BOOL &bHandled) {
  bHandled = FALSE;
#if 0
	if (m_bDoModal)
	{ 
		EndDialog(IDCANCEL); 
	}

	m_syncWindow._OnNcDestroy();

//	if (!IsChildWindow())
	{
		TopWindowManager* pTopWndMgr = GetUIApplication()->GetTopWindowMgr();
		if (pTopWndMgr)
			pTopWndMgr->RemoveTopWindowObject(this);
	}

    this->m_oDragDropManager.OnWindowDestroy();
	m_oMouseManager.HandleMessage( uMsg, wParam, lParam, &bHandled);
#endif
  if (m_oldWndProc) {
    ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_oldWndProc);
    m_oldWndProc = nullptr;
  }
  set_hwnd(nullptr);

  m_ui_window.onDestroy();

#if 0
	this->DestroyChildObject();   // 删除子对象
    
    if (m_pCallbackProxy)
    {
        m_pCallbackProxy->OnWindowDestroy();
    }
#endif
  return 0;
}

// win7下面带WS_THICKFRAME样式窗口贴边最大化/还原的消息只有WM_SIZE，没有WM_SYSCOMMAND
// 因此就不能使用WM_SYSCOMMAND消息来处理
LRESULT WindowPlatformWin::_OnSize(unsigned int uMsg, WPARAM wParam,
                                   LPARAM lParam, BOOL &bHandled) {
  bHandled = FALSE;
  if (SIZE_MINIMIZED == wParam)
    return 0;

  m_ui_window.onSize(LOWORD(lParam), HIWORD(lParam));
  return 0;
}

LRESULT WindowPlatformWin::_OnHandleMouseMessage(unsigned int uMsg,
                                                 WPARAM wParam, LPARAM lParam,
                                                 BOOL &bHandled) {
  int x = GET_X_LPARAM(lParam);
  int y = GET_Y_LPARAM(lParam);
  
  // 消息中拿到的坐标是像素坐标，我们的控件坐标都是独立坐标。
  m_ui_window.m_dpi.Restore(x);
  m_ui_window.m_dpi.Restore(y);

  if (uMsg == WM_MOUSEMOVE) {
    m_ui_window.m_mouse_key.OnMouseMove(x, y);
  }
  else if (uMsg == WM_LBUTTONDOWN) {
    m_ui_window.m_mouse_key.OnLButtonDown(x, y);
  } else if (uMsg == WM_LBUTTONUP) {
    m_ui_window.m_mouse_key.OnLButtonUp(x, y);
  }
  bHandled = TRUE;
#if 0
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
#endif
  return 0;
}

#define DEFAULT_SCREEN_DPI 96 // USER_DEFAULT_SCREEN_DPI

int WindowPlatformWin::GetDpi() {
  if (0 == m_dpi) {
    // 先检测用户是否禁用了DPI缩放
    bool bDisableScale = false;

    ATL::CRegKey reg;
    if (ERROR_SUCCESS ==
        reg.Open(HKEY_CURRENT_USER,
                 TEXT("Software\\Microsoft\\Windows "
                      "NT\\CurrentVersion\\AppCompatFlags\\Layers"),
                 KEY_READ)) {
      // REG_SZ
      wchar_t szExePath[MAX_PATH] = {0};
      GetModuleFileName(nullptr, szExePath, MAX_PATH - 1);

      wchar_t szValue[256] = {0};
      unsigned long count = 256;
      if (ERROR_SUCCESS == reg.QueryStringValue(szExePath, szValue, &count)) {
        if (_tcsstr(szValue, TEXT("HIGHDPIAWARE"))) {
          bDisableScale = true;
        }
      }
    }

    if (bDisableScale) {
      m_dpi = DEFAULT_SCREEN_DPI;
    } else {
      // 由程序自己实现DPI缩放，不用系统帮忙。
      HMODULE hModule = GetModuleHandle(L"User32.dll");
      if (hModule) {
        // SetProcessDPIAware();
        FARPROC f = GetProcAddress(hModule, "SetProcessDPIAware");
        if (f)
          f();
      }

      HDC hDC = GetDC(nullptr);
      m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
      ReleaseDC(nullptr, hDC);
    }
  }
  return m_dpi;
}

float WindowPlatformWin::GetDpiScale() {
  float fScale = (float)GetDpi() / DEFAULT_SCREEN_DPI;
  return fScale;
}

int WindowPlatformWin::ScaleByDpi(int x) {
  if (0 == x)
    return 0;

  if (GetDpi() == DEFAULT_SCREEN_DPI)
    return x;

  return _round(x * GetDpiScale());
}
int WindowPlatformWin::RestoreByDpi(int x) {
  if (0 == x)
    return 0;

  if (GetDpi() == DEFAULT_SCREEN_DPI)
    return x;

  return _round(x / GetDpiScale());
}

// 宽度、高度小于0时，如AUTO/NDEF不适应于dpi
int WindowPlatformWin::ScaleByDpi_if_gt0(int x) {
  if (x <= 0)
    return x;

  return ScaleByDpi(x);
}

int WindowPlatformWin::RestoreByDpi_if_gt0(int x) {
  if (x <= 0)
    return x;

  return RestoreByDpi(x);
}

} // namespace ui