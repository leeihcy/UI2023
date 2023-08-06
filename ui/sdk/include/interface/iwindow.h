#ifndef _INCLUDED_IWINDOW_
#define _INCLUDED_IWINDOW_

#include "include/macro/uidefine.h"
#include "../common/signalslot/signal.h"
#include "iobject.h"
#include <SkCanvas.h>
#include <SkSurface.h>
// #include "irenderlayer.h"
// #include "ipanel.h"

namespace ui {
#if 0
struct IWindowMouseMgr;
struct IWindowKeyboardMgr;

//
//  非ui消息，采用::SendMessage发送
//  处理一个同步移动窗口事件(添加、修改、删除)
//		wparam:  SYNC_WINDOW_EVENT_TYPE
//		lparam:  具体查看SYNC_WINDOW_EVENT_TYPE定义
#define UI_WM_SYNC_WINDOW (WM_USER + 827)

#define ANCHOR_NONE 0
#define ANCHOR_LEFT 0x0001
#define ANCHOR_RIGHT 0x0002
#define ANCHOR_TOP 0x0004
#define ANCHOR_BOTTOM 0x0008
#define ANCHOR_OUT_LEFT 0x0010
#define ANCHOR_OUT_RIGHT 0x0020
#define ANCHOR_OUT_TOP 0x0040
#define ANCHOR_OUT_BOTTOM 0x0080
#define ANCHOR_CUSTOM                                                          \
  0x0100 // 发送UI_WM_SYNC_WINDOWPOSCHANGING消息给窗口，由窗口自行决定如何移动自己
#define ANCHOR_CLIENTREGION                                                    \
  0x200 // 与host的客户区对齐，例如host是个普通边框窗口


enum SYNC_WINDOW_EVENT_TYPE
{
    ADD_SYNC_WINDOW,            // 通知HOST, 增加一个跟随者.long: SyncWindowData*
    MODIFY_SYNC_WINDOW,         // 通知HOST, 修改一个跟随者信息.long: SyncWindowData*
    REMOVE_SYNC_WINDOW,         // 通知HOST, 移除一个跟随者.long: SyncWindowData*

    ADD_SYNC_WINDOW_RESULT,     // 通知SITE，添加成功.long: host HWND
    MODIFY_SYNC_WINDOW_RESULT,  // 通知SITE，修改成功.long: host HWND
    REMOVE_SYNC_WINDOW_RESULT,  // 通知SITE，删除成功.long: host HWND

    HOST_WINDOW_DESTROYED,      // 要跟随的HOST销毁了，清空自己的HOST句柄. long: 无
    HOST_WINDOW_POSCHANGING,    // 发送给SITE窗口，标明下一次响应的WindowPosChaning消息是因为HOST移动产生的

    SYNC_NOW,                   // 发送给host，立即同步一次，如窗口刚创建完成
};

struct  AnchorData
{
    int  xOffset;
    int  yOffset;

    union{
        int xOffset2;
        int Width;         // -1 表示不修改窗口宽度
    };
    union{
        int yOffset2;
        int Height;       // -1 表示不修改窗口高度
    };

    AnchorData()
    {
        xOffset = yOffset = xOffset2 = yOffset2 = 0;
    }
};

#define SWDS_MASK_ANCHORTYPE 0x1
#define SWDS_MASK_ANCHORDATA 0x2
#define SWDS_MASK_ANCHORON 0x4
#define SWDS_MASK_SYNC_VISIBLE 0x8
#define SWDS_MASK_ALL 0xFFFF

struct  SyncWindowData
{
    HWND         m_hWnd;           
    unsigned int         m_nMask;
    unsigned int         m_nAnchorType;   
    AnchorData   m_rcAnchorData;  // 当m_nAnchorType相应位有值时，m_rcAnchorData的该位有效
    bool         m_bAnchorOn;     // 跟随是否起作用。例如临时将窗口移出HOST窗口，不进行跟随，然后又贴到HOST窗口旁再次开启跟随。
	bool         m_bSyncVisible;  // 是否同步显隐

    SyncWindowData()
    {
        m_hWnd = nullptr;
        m_nAnchorType = ANCHOR_NONE;
        m_bAnchorOn = true;
		m_bSyncVisible = false;

        m_nMask = 0;
    }
};

// 用于外部业务实现者处理窗口消息。
// 外部不再要求从IWindowBase派生，只需要实现IWindowMessageCallback接口即可。
struct IWindowDelegate
{
    virtual bool  OnWindowMessage(unsigned int, long, long, LRESULT& lResult) { return FALSE; }
    virtual bool  OnWindowUIMessage(UIMSG* pMsg) { return FALSE; }
    virtual void  OnWindowInitialize() {}
	virtual void  DoBindPlz(bool bind) {}  // 在换肤时会再通知一次，因为有可能有新增的控件
    virtual void  OnWindowClose(bool& bCanClose) { }
    virtual void  OnWindowDestroy() {};
};

class WindowBase;
struct UIAPI_UUID(1C7CED21-3CF6-49C9-9E52-72522C8A1CF6) IWindowBase
 : public IPanel
{
    HWND  GetHWND();
    IWindowRender*  GetWindowRender();
	IResBundle*  GetSkinRes();

    bool  IsChildWindow();
	bool  IsWindowVisible();
    void  ShowWindow();
    void  HideWindow();
    bool  IsDoModal();
    void  CenterWindow(HWND hWndCenter = nullptr);
    void  CenterWindow(HMONITOR hMonitor = nullptr);
	void  GetWindowNormalRect(Rect* prc);
	void  SetWindowNormalRect(const Rect* prc);
	void  UpdateDesktopLayout();

    void  SetFocusObject(IObject* pObj);
	void  SetPressObject(IObject* pObj);
    IObject*  GetHoverObject();
    IObject*  GetPressObject();
    IObject*  GetObjectByPos(IObject* pObjParent, Point* pt, bool bSkinBuilderInvoke=false);

    bool  Create(const wchar_t* szID, HWND hWndParent=nullptr, Rect* prc=nullptr, long lStyle = 0, long lExStyle = 0);
    bool  Attach(HWND hWnd, const wchar_t* szID);
    void  Detach();
	INT_PTR  DoModal(const wchar_t* szID, HWND hWndParent, bool canResize);
	INT_PTR  DoModal(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szID, HWND hWndParent);
    HWND  DoModeless(const wchar_t* szID, HWND hWndParent, bool canResize);
    HWND  DoModeless(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szID, HWND hWndParent);
    void  EndDialog(INT_PTR);
	void  DestroyWindow();
	void  ChangeSkinLayout(const wchar_t* szLayoutId);

    void  SaveMemBitmap(wchar* szFile);
	void  AlphaBlendMemBitmap(HDC hDC, Rect* prc, int alpha);
	void  BitBltMemBitmap(HDC hDC, Rect* prc);
    void  EnableDwmTransition(bool b);
	void  EnableGpuComposite(bool b);
	bool  IsGpuComposite();
	void  DirectComposite();
	void  SetWindowMessageCallback(IWindowDelegate*);

    void  CalcWindowSizeByClientSize( Size sizeClient, Size* pSizeWindow );
    void  CalcClientRectByWindowRect( Rect* rcWindow, Rect* rcClient );

//     bool  AddAnchorItem(const SyncWindowData& data);
//     void  HideAllAnchorItem();

    long  SetDroppable(bool b);
	bool  IsSizeMoveIng();

	signal_mc<long>&  SizeChangedEvent();
    signal_mc<bool&>&  OnCloseEvent();  // void  OnWindowClose(bool& bCanClose);
    UI_DECLARE_INTERFACE(WindowBase);
};

#endif


// window style
typedef struct tagWindowStyle
{
	bool  destroyed : 1;       // 表示该窗口已经被销毁了(WM_NCDESTROY)，用于触发OnFinalMessage
	bool  attach : 1;          // 表示该窗口是attach的，创建、销毁由外部来控制
	bool  setcreaterect : 1;   // 创建窗口时指定了窗口大小，不用再进行窗口布局了
	bool  dialog_noresize : 1; // 用于解决win7下面Dialog显示大小于GetWindowRect不一致的问题
	bool  hard_composite: 1;   // 本窗口使用硬件合成 
}WindowStyle;


class Window;
struct UIAPI IWindow : public IObject {
  void Create(const Rect &rect);
  void SetTitle(const char *title);
  void Show();

  signal<void()> &DestroySignal();
  signal<void(IRenderTarget*)> &PaintSignal();

  UI_DECLARE_INTERFACE(Window)
  UI_DEFINE_CLASS_GUID("5C36801E-5929-4512-A998-F9719DCC6903");
};

// 辅助类
class WindowPtr {
public:
  WindowPtr(IResBundle *bundle) { m_p = IWindow::CreateInstance(bundle); }
  ~WindowPtr() {
    if (m_p) { m_p->Release(); }
  }
  WindowPtr(const WindowPtr&) = delete;
  WindowPtr& operator=(const WindowPtr&) = delete;

  IWindow *operator->() { return m_p; }
  operator IWindow *() const { return m_p; }

private:
  IWindow *m_p;
};

// #if 0
// extern "C"
// void UIAPI GetWindowNormalRect(HWND hWnd, Rect* prc);
// #endif
} // namespace ui

#endif // _INCLUDED_IWINDOW_