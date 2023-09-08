#pragma once

#include "Src\Helper\MouseKeyboard\mousemanager.h"
#include "Src\Helper\dragdrop\windowdragdropmgr.h"
#include "Src\Layer\windowrender.h"
#include "Src\UIObject\Panel\panel.h"
#include "Src\UIObject\Window\sync_window.h"
#include "Src\Util\RectArray\rectarray.h"
#include "include/interface/iwindow.h"

namespace ui {
class WindowRender;
class Resource;
interface IRenderFont;

// 外部消息回调包装。通过hook的方式实现
class CWindowMessageHookProxy : public IMessage {
public:
  CWindowMessageHookProxy(IWindowDelegate *p) : IMessage(CREATE_IMPL_TRUE) {
    m_pCallback = p;
  }
  BOOL OnWindowMessage(unsigned int msg, WPARAM w, LPARAM l, long &lResult) {
    lResult = 0;
    if (m_pCallback) {
      return m_pCallback->OnWindowMessage(msg, w, l, lResult);
    }
    return FALSE;
  }
  void OnWindowClose(bool &bCanClose) {
    if (m_pCallback) {
      m_pCallback->OnWindowClose(bCanClose);
    }
  }
  void OnWindowDestroy() {
    if (m_pCallback) {
      m_pCallback->OnWindowDestroy();
    }
  }
  void OnWindowInit() {
    if (m_pCallback) {
      m_pCallback->OnWindowInitialize();
    }
  }

  void DoBindPlz(bool bind) {
    if (m_pCallback) {
      m_pCallback->DoBindPlz(bind);
    }
  }

protected:
  virtual BOOL virtualProcessMessage(UIMSG *pMsg, int nMsgMapID, bool bDoHook) {
    if (m_pCallback)
      return m_pCallback->OnWindowUIMessage(pMsg);
    return FALSE;
  }

private:
  IWindowDelegate *m_pCallback;
};

class WindowBase : public Panel {
public:
  WindowBase(IWindowBase *p);
  ~WindowBase();


public:
  operator HWND() const;
  HWND GetHWND();

  Object *GetHoverObject();
  Object *GetPressObject();
  void SetPressObject(Object *);

  bool Create(const wchar_t *szID, HWND hWndParent = nullptr,
              RECT *prc = nullptr, long lStyle = 0, long lExStyle = 0);
  void DestroyWindow();
  INT_PTR DoModal(const wchar_t *szID, HWND hWndParent, bool canResize);
  INT_PTR DoModal(HINSTANCE hResInst, unsigned int nResID, const wchar_t *szID,
                  HWND hWndParent);
  HWND DoModeless(const wchar_t *szID, HWND hWndOnwer, bool canResize);
  HWND DoModeless(HINSTANCE hResInst, unsigned int nResID, const wchar_t *szID,
                  HWND hWndOnwer);
  void EndDialog(INT_PTR);
  bool Attach(HWND hWnd, const wchar_t *szID);
  void Detach();
  void SetWindowMessageCallback(IWindowDelegate *);

  void ShowWindow();
  void HideWindow();
  void SetActive(bool b);
  bool IsActive();
  bool IsDoModal();
  bool IsSizeMoveIng();
  HRESULT SetCanDrop(bool b);
  void CenterWindow(HWND hWndCenter = nullptr);
  void CenterWindow(HMONITOR hMonitor = nullptr);
  void UpdateDesktopLayout();
  void GetWindowNormalRect(LPRECT prc);
  void SetWindowNormalRect(LPCRECT prc);
  void AsyncTask(std::function<void(WindowBase *)> callback);

  void CalcWindowSizeByClientSize(SIZE sizeClient, SIZE *pSizeWindow);
  void CalcClientRectByWindowRect(RECT *rcWindow, RECT *rcClient);
  void Control_NotifyMe(const String &idPath, int nNotifyMapID);
  void Control_NotifyMe(Object *pObj, int nNotifyMapID);

  WindowMouseMgr *GetMouseMgr() { return &m_oMouseManager; }
  Object *GetObjectUnderCursor();
  Object *GetObjectByPos(Object *pObjParent, POINT *pt,
                         bool bSkinBuilderInvoke = false);
  void SetFocusObject(Object *pObj);

  // 获取字体流程：先获取自己的m_pTextRender，如果没有则调用自己的m_pWindow的GetRenderFont
  IRenderFont *GetWindowDefaultRenderFont();
  void SetDefaultRenderFont(const wchar_t *);
  const wchar_t *GetDefaultRenderFontId();

  void SaveMemBitmap(TCHAR *szFile);
  void AlphaBlendMemBitmap(HDC hDC, RECT *prc, int alpha);
  void BitBltMemBitmap(HDC hDC, RECT *prc);
  void EnableDwmTransition(bool b);
  void SetCursorByHitTest(unsigned int nHitTest);

  void ChangeSkinLayout(const wchar_t *szLayoutId);

protected:
  bool CreateUI(const wchar_t *szID);
  INT_PTR ModalLoop(HWND hWndParent);
  HMONITOR GetPrimaryMonitor();

public:
  virtual void virtualInnerInitWindow();

  virtual void SetObjectPos(int x, int y, int cx, int cy,
                            int nFlag = 0) override;
  virtual void virtualSetVisibleEx(VISIBILITY_TYPE eType) override;
  virtual void virtualSetEnable(bool b) override;
  virtual bool IsVisible() override;
  virtual bool IsEnable() override;
  virtual IMKMgr *virtualGetIMKMgr() override;

protected:

#pragma region // message handle
public:
  static long CALLBACK StartDialogProc(HWND hwnd, unsigned int uMsg,
                                       WPARAM wParam, LPARAM lParam);

protected:
  
  long DefWindowProc(unsigned int uMsg, WPARAM wParam, LPARAM lParam);


protected:
  long _OnSetCursor(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                    BOOL &bHandled);
  long _OnNcHitTest(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                    BOOL &bHandled);
  long _OnPostMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                      BOOL &bHandled);
  long _OnCreate(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                 BOOL &bHandled);
  long _OnClose(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                BOOL &bHandled);

  long _OnHandleMouseMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                             BOOL &bHandled);
  long _OnHandleKeyBoardMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                                BOOL &bHandled);
  long _OnHandleTouchMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                             BOOL &bHandled);
  long _OnSetFocus(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                   BOOL &bHandled);
  long _OnKillFocus(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                    BOOL &bHandled);
  long _OnMouseActive(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                      BOOL &bHandled);
  long _OnThemeChange(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                      BOOL &bHandled);
  long _OnWindowPosChanging(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                            BOOL &bHandled);
  long _OnShowWindow(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                     BOOL &bHandled);
  long _OnSyncWindow(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                     BOOL &bHandled);
  long _OnGetMinMaxInfo(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                        BOOL &bHandled);
  long _OnEnterSizeMove(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                        BOOL &bHandled);
  long _OnExitSizeMove(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                       BOOL &bHandled);
  long _OnGetObject(unsigned int uMsg, WPARAM wParam, LPARAM lParam,
                    BOOL &bHandled);

  void OnGetDesiredSize(SIZE *pSize);

#pragma endregion

public:
  HWND m_hWnd; // 窗口句柄

protected:
  

  SyncWindow m_syncWindow;
  WindowDragDropMgr m_oDragDropManager; // 拖拽管理器
  WindowMouseMgr m_oMouseManager;       // 鼠标消息处理器
  WindowRender
      m_oWindowRender; // 窗口的渲染放在这个对象里面，windowbase不负责渲染

  CWindowMessageHookProxy *m_pCallbackProxy;

  bool m_bDoModal; // 模态窗口数据
  bool m_bEndModal;
  INT_PTR m_lDoModalReturn;

  bool m_bFirsetEraseBkgnd; // TODO: 解决第一次显示窗口时，会闪烁一下黑色的问题.

  // 机制：如果控件没有配置字体时，默认取窗口的字体，即m_pDefaultFont
  //
  // 1. 当xml中配置了窗口字体时，m_hFont为xml中的字体。
  // 2. 当没有配置字体时，使用窗口字体。
  // 3. 当窗口字体也没时，使用UI Font Res 默认字体
  // 4. 最后则尝试使用system default font.
  IRenderFont *m_pDefaultFont;

public:
  // 事件
  signal_mc<long> size_changed;
  signal_mc<bool &> on_close;
};

} // namespace ui
