#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_WIN_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_WIN_H_
#include "window.h"

#include "src/util/windows.h"
#include <string>

namespace ui {

//
//	用于在创建窗口的时候，拦截第一个窗口消息，并进行子类化
//
class CREATE_WND_DATA // 等同于 ATL 中的_ATL_WIN_MODULE70
{
public:
  CREATE_WND_DATA();
  ~CREATE_WND_DATA();

public:
  void AddCreateWndData(_AtlCreateWndData *pData, void *pThis);
  void *ExtractCreateWndData();

private:
  CRITICAL_SECTION m_cs;
  _AtlCreateWndData *m_pCreateWndList;
};

class WindowPlatformWin : public WindowPlatform {
public:
  WindowPlatformWin(ui::Window &w);
  ~WindowPlatformWin();
  void Initialize() override;

  bool Create(const Rect &rect) override;
  bool CreateTransparent(const Rect &rect);
  void Destroy();

  void SetTitle(const char *title) override;
  std::string GetTitle();
  void SetBorderless(bool no_border = true);
  void SetMinMaxSize(int wMin, int hMin, int wMax, int hMax);
  void SetWindowRect(int x, int y, int width, int height);
  void GetClientRect(Rect *prect);
  void GetWindowRect(Rect *prect);
  void CenterWindow();
  bool IsChildWindow() override;
  bool IsWindowVisible() override;

  void Attach(HWND window);
  HWND Detach();

  void Show() override;
  void Hide();

  void Invalidate(const Rect *prect) override;
  void Commit(IRenderTarget *pRT, const Rect *prect, int count) override;

public:
  // WndProc的原始消息处理   // 经过virtual扩展了
  // VIRTUAL_BEGIN_MSG_MAP(WindowPlatformWin)
  BEGIN_MSG_MAP_EX(WindowPlatformWin)
  MESSAGE_HANDLER(WM_ERASEBKGND, _OnEraseBkgnd)
  MESSAGE_HANDLER(WM_PAINT, _OnPaint)
#if 0
  MESSAGE_HANDLER(WM_SETCURSOR, _OnSetCursor)
  MESSAGE_HANDLER(WM_NCHITTEST, _OnNcHitTest)
  MESSAGE_HANDLER(UI_MSG_POSTMESSAGE, _OnPostMessage)
#endif

  // MESSAGE_HANDLER(WM_MOUSEMOVE, _OnHandleMouseMessage)
  // MESSAGE_HANDLER(WM_MOUSELEAVE, _OnHandleMouseMessage)
  MESSAGE_HANDLER(WM_LBUTTONDOWN, _OnHandleMouseMessage)
  MESSAGE_HANDLER(WM_LBUTTONUP, _OnHandleMouseMessage)
  // MESSAGE_HANDLER(WM_RBUTTONDOWN, _OnHandleMouseMessage)
  // MESSAGE_HANDLER(WM_RBUTTONUP, _OnHandleMouseMessage)
  // MESSAGE_HANDLER(WM_LBUTTONDBLCLK, _OnHandleMouseMessage)
  // MESSAGE_HANDLER(WM_MOUSEWHEEL, _OnHandleMouseMessage)
#if 0
  MESSAGE_HANDLER(WM_CANCELMODE, _OnHandleMouseMessage)

  MESSAGE_HANDLER(WM_CHAR, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_KEYDOWN, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_KEYUP, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_SYSKEYDOWN, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_SYSKEYUP, _OnHandleKeyBoardMessage)

#if (WINVER >= 0x0601)
  MESSAGE_HANDLER(/*WM_TOUCH*/ 0x0240, _OnHandleTouchMessage)
  MESSAGE_HANDLER(/*WM_GESTURE*/ 0x0119, _OnHandleTouchMessage)
  MESSAGE_HANDLER(/*WM_GESTURENOTIFY*/ 0x011A, _OnHandleTouchMessage)
#endif

  MESSAGE_HANDLER(WM_IME_STARTCOMPOSITION, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_ENDCOMPOSITION, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_COMPOSITION, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_SETCONTEXT, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_NOTIFY, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_CONTROL, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_COMPOSITIONFULL, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_SELECT, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_CHAR, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_REQUEST, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_KEYDOWN, _OnHandleKeyBoardMessage)
  MESSAGE_HANDLER(WM_IME_KEYUP, _OnHandleKeyBoardMessage)

  MESSAGE_HANDLER(WM_GETMINMAXINFO, _OnGetMinMaxInfo)
  MESSAGE_HANDLER(WM_SHOWWINDOW, _OnShowWindow)
  MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, _OnWindowPosChanging)
  MESSAGE_HANDLER(UI_WM_SYNC_WINDOW, _OnSyncWindow)
  MESSAGE_HANDLER(WM_ENTERSIZEMOVE, _OnEnterSizeMove)
  MESSAGE_HANDLER(WM_EXITSIZEMOVE, _OnExitSizeMove)
  MESSAGE_HANDLER(WM_SETFOCUS, _OnSetFocus)
  MESSAGE_HANDLER(WM_KILLFOCUS, _OnKillFocus)
  MESSAGE_HANDLER(WM_MOUSEACTIVATE, _OnMouseActive)
  MESSAGE_HANDLER(WM_GETOBJECT, _OnGetObject)
  MESSAGE_HANDLER(WM_CREATE, _OnCreate)
  MESSAGE_HANDLER(WM_INITDIALOG, _OnCreate)
  MESSAGE_HANDLER(WM_CLOSE, _OnClose)
#endif
  MESSAGE_HANDLER(WM_SIZE, _OnSize)
  MESSAGE_HANDLER(WM_NCDESTROY, _OnNcDestroy)
  MESSAGE_HANDLER(WM_DESTROY, _OnDestroy)
#if 0
#if (_WIN32_WINNT >= 0x0501)
  MESSAGE_HANDLER(WM_THEMECHANGED, _OnThemeChange)
#endif
#endif
  END_MSG_MAP()

  LRESULT StartProc(HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                    bool bWindowOrDialog);

  static LRESULT CALLBACK ThunkWndProc(HWND hwnd, unsigned int uMsg,
                                       WPARAM wParam, LPARAM lParam);

  void set_hwnd(HWND hWnd) {
    m_hWnd = hWnd;
    // m_syncWindow.SetHwnd(hWnd);
  }
  virtual bool PreTranslateMessage(unsigned int uMsg, WPARAM wParam,
                                   LPARAM lParam, LRESULT *pRet);

  LRESULT WndProc(unsigned int uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT WndProc_GetRetValue(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                              BOOL bHandled, LRESULT lRet);

private:
  LRESULT _OnSize(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                  BOOL &bHandled);

  LRESULT _OnEraseBkgnd(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                        BOOL &bHandled);
  LRESULT _OnPaint(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                   BOOL &bHandled);
  LRESULT _OnNcDestroy(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                       BOOL &bHandled);
  LRESULT _OnDestroy(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                     BOOL &bHandled);
  LRESULT _OnHandleMouseMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                                BOOL &bHandled);

public:
  // 创建窗口时，拦截第一个窗口消息，将HWND->this
  static CREATE_WND_DATA s_create_wnd_data;

private:
  CWndProcThunk m_thunk; // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
  WNDPROC m_oldWndProc; // 该窗口的原始窗口过程

  ui::Window &m_ui_window;
  HWND m_hWnd;
};

} // namespace ui

#endif