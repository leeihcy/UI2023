#pragma once

#include "Src\UIObject\Panel\panel.h"
#include "Src\UIObject\Window\sync_window.h"
#include "include/interface/iwindow.h"
#include "Src\Helper\MouseKeyboard\mousemanager.h"
#include "Src\Helper\dragdrop\windowdragdropmgr.h"
#include "Src\Util\RectArray\rectarray.h"
#include "Src\Layer\windowrender.h"

namespace ui
{
class WindowRender;
class ResBundle;
interface IRenderFont;

// 
//	用于在创建窗口的时候，拦截第一个窗口消息，并进行子类化
//
class CREATE_WND_DATA  // 等同于 ATL 中的_ATL_WIN_MODULE70
{
public:
	CREATE_WND_DATA();
	~CREATE_WND_DATA();

public:
	void  AddCreateWndData(_AtlCreateWndData* pData, void* pThis);
	void* ExtractCreateWndData();

private:
	CRITICAL_SECTION   m_cs;
	_AtlCreateWndData* m_pCreateWndList;
};

// 外部消息回调包装。通过hook的方式实现
class CWindowMessageHookProxy : public IMessage
{
public:
	CWindowMessageHookProxy(IWindowDelegate* p) : IMessage(CREATE_IMPL_TRUE)
	{
		m_pCallback = p;
	}
	BOOL  OnWindowMessage(unsigned int msg, WPARAM w, LPARAM l, long& lResult)
	{
		lResult = 0;
		if (m_pCallback)
		{
			return m_pCallback->OnWindowMessage(msg, w, l, lResult);
		}
		return FALSE;
	}
    void  OnWindowClose(bool& bCanClose)
    {
        if (m_pCallback)
        {
            m_pCallback->OnWindowClose(bCanClose);
        }
    }
    void  OnWindowDestroy()
    {
        if (m_pCallback)
        {
            m_pCallback->OnWindowDestroy();
        }
    }
    void  OnWindowInit()
    {
        if (m_pCallback)
        {
            m_pCallback->OnWindowInitialize();
        }
    }

	void  DoBindPlz(bool bind)
	{
		if (m_pCallback)
		{
			m_pCallback->DoBindPlz(bind);
		}
	}
protected:
	virtual BOOL  virtualProcessMessage(
                UIMSG* pMsg, int nMsgMapID, bool bDoHook)
	{
		if (m_pCallback)
			return m_pCallback->OnWindowUIMessage(pMsg);
		return FALSE;
	}
private:
	IWindowDelegate*  m_pCallback;
};

class WindowBase : public Panel
{
public:
    WindowBase(IWindowBase* p);
    ~WindowBase();

    // WndProc的原始消息处理   // 经过virtual扩展了
    VIRTUAL_BEGIN_MSG_MAP(WindowBase) 
        MESSAGE_HANDLER( WM_ERASEBKGND,           _OnEraseBkgnd )
        MESSAGE_HANDLER( WM_PAINT,                _OnPaint )
        MESSAGE_HANDLER( WM_SETCURSOR,            _OnSetCursor )
        MESSAGE_HANDLER( WM_NCHITTEST,            _OnNcHitTest )
		MESSAGE_HANDLER( UI_MSG_POSTMESSAGE,       _OnPostMessage )
        
        MESSAGE_HANDLER( WM_MOUSEMOVE,            _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_MOUSELEAVE,           _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_LBUTTONDOWN,          _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_LBUTTONUP,            _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_RBUTTONDOWN,          _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_RBUTTONUP,            _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_LBUTTONDBLCLK,        _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_MOUSEWHEEL,           _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_CANCELMODE,           _OnHandleMouseMessage )

        MESSAGE_HANDLER( WM_CHAR,                 _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_KEYDOWN,              _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_KEYUP,                _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_SYSKEYDOWN,           _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_SYSKEYUP,             _OnHandleKeyBoardMessage )

#if(WINVER >= 0x0601)
        MESSAGE_HANDLER( /*WM_TOUCH*/0x0240,      _OnHandleTouchMessage )
        MESSAGE_HANDLER( /*WM_GESTURE*/0x0119,    _OnHandleTouchMessage )
        MESSAGE_HANDLER( /*WM_GESTURENOTIFY*/0x011A, _OnHandleTouchMessage )
#endif

        MESSAGE_HANDLER( WM_IME_STARTCOMPOSITION, _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_ENDCOMPOSITION,   _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_COMPOSITION,      _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_SETCONTEXT,       _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_NOTIFY,           _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_CONTROL,          _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_COMPOSITIONFULL,  _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_SELECT,           _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_CHAR,             _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_REQUEST,          _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_KEYDOWN,          _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_KEYUP,            _OnHandleKeyBoardMessage )
        
        MESSAGE_HANDLER( WM_GETMINMAXINFO,        _OnGetMinMaxInfo )
        MESSAGE_HANDLER( WM_SHOWWINDOW,            _OnShowWindow)
        MESSAGE_HANDLER( WM_WINDOWPOSCHANGING,    _OnWindowPosChanging )
        MESSAGE_HANDLER( UI_WM_SYNC_WINDOW,       _OnSyncWindow )
        MESSAGE_HANDLER( WM_ENTERSIZEMOVE,        _OnEnterSizeMove )
        MESSAGE_HANDLER( WM_EXITSIZEMOVE,         _OnExitSizeMove )
        MESSAGE_HANDLER( WM_SIZE,                 _OnSize )
        MESSAGE_HANDLER( WM_SETFOCUS,             _OnSetFocus )
        MESSAGE_HANDLER( WM_KILLFOCUS,            _OnKillFocus )
        MESSAGE_HANDLER( WM_MOUSEACTIVATE,        _OnMouseActive )
		MESSAGE_HANDLER( WM_GETOBJECT,            _OnGetObject )
        MESSAGE_HANDLER( WM_CREATE,               _OnCreate )
        MESSAGE_HANDLER( WM_INITDIALOG,           _OnCreate )
		MESSAGE_HANDLER( WM_CLOSE,                _OnClose)
		MESSAGE_HANDLER( WM_NCDESTROY,            _OnNcDestroy )
        MESSAGE_HANDLER( WM_DESTROY,              _OnDestroy )
#if(_WIN32_WINNT >= 0x0501)
        MESSAGE_HANDLER( WM_THEMECHANGED,         _OnThemeChange )
#endif
    END_MSG_MAP()

    // UI事件的消息处理
	UI_BEGIN_MSG_MAP()
        UIMSG_ERASEBKGND( OnEraseBkgnd )
        UIMSG_DM_GETDEFID( OnGetDefId )
        UIMSG_DM_SETDEFID( OnSetDefId )
        UIMSG_GETDESIREDSIZE( OnGetDesiredSize )
        UIMSG_QUERYINTERFACE( WindowBase )
        UIMSG_SERIALIZE( OnSerialize )
        UIMSG_FINALCONSTRUCT( FinalConstruct )
        UIMSG_PRECREATEWINDOW( PreCreateWindow )
     UI_END_MSG_MAP_CHAIN_PARENT( Panel )

    
public:
	operator   HWND() const;
    HWND       GetHWND();

    Object*    GetHoverObject();
    Object*    GetPressObject();
	void  SetPressObject(Object*);

	bool  Create(const wchar_t* szID,  HWND hWndParent = nullptr, RECT* prc = nullptr, long lStyle = 0, long lExStyle = 0);
	void  DestroyWindow();
	INT_PTR  DoModal(const wchar_t* szID, HWND hWndParent, bool canResize);
	INT_PTR  DoModal(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szID, HWND hWndParent);
	HWND  DoModeless(const wchar_t* szID, HWND hWndOnwer, bool canResize);
	HWND  DoModeless(HINSTANCE hResInst, unsigned int nResID, const wchar_t* szID, HWND hWndOnwer);
	void  EndDialog(INT_PTR);
	bool  Attach(HWND hWnd, const wchar_t* szID);
	void  Detach();
	void  SetWindowMessageCallback(IWindowDelegate*);

	void  ShowWindow();
	void  HideWindow();
    void  SetActive(bool b);
    bool  IsActive();
    bool  IsDoModal();
	bool  IsSizeMoveIng();
    HRESULT  SetCanDrop(bool b);
    void  CenterWindow(HWND hWndCenter = nullptr);
    void  CenterWindow(HMONITOR hMonitor = nullptr);
    void  UpdateDesktopLayout();
	void  GetWindowNormalRect(LPRECT prc);
	void  SetWindowNormalRect(LPCRECT prc);
	void  AsyncTask(std::function<void(WindowBase*)> callback);

	void  CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow );
	void  CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient );
	void  Control_NotifyMe(const String&  idPath, int nNotifyMapID);
	void  Control_NotifyMe(Object*  pObj, int nNotifyMapID);

    WindowMouseMgr*  GetMouseMgr() { return &m_oMouseManager; }
    Object*  GetObjectUnderCursor();
    Object*  GetObjectByPos(Object* pObjParent, POINT* pt, bool bSkinBuilderInvoke=false);
    void  SetFocusObject(Object* pObj);

    // 获取字体流程：先获取自己的m_pTextRender，如果没有则调用自己的m_pWindow的GetRenderFont
    IRenderFont*  GetWindowDefaultRenderFont();
	void  SetDefaultRenderFont(const wchar_t*);
	const wchar_t*  GetDefaultRenderFontId();


	void  SaveMemBitmap(TCHAR* szFile);
	void  AlphaBlendMemBitmap(HDC hDC, RECT* prc, int alpha);
	void  BitBltMemBitmap(HDC hDC, RECT* prc);
    void  EnableDwmTransition(bool b);
    void  SetCursorByHitTest(unsigned int nHitTest);

	void  ChangeSkinLayout(const wchar_t* szLayoutId);

protected:
	bool  CreateUI(const wchar_t* szID);
	INT_PTR  ModalLoop(HWND hWndParent);
    HMONITOR  GetPrimaryMonitor();
    
public:
    virtual void  virtualInnerInitWindow();
	
	virtual void  SetObjectPos( int x, int y, int cx, int cy, int nFlag=0) override;
	virtual void  virtualSetVisibleEx(VISIBILITY_TYPE eType) override;
	virtual void  virtualSetEnable(bool b) override;
	virtual bool  IsVisible() override;
	virtual bool  IsEnable() override;
	virtual IMKMgr* virtualGetIMKMgr() override;

protected:
	virtual BOOL  PreTranslateMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam, long* pRet);
	
#pragma region // message handle
public:
	static long CALLBACK  StartWindowProc( HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam );
	static long CALLBACK  StartDialogProc( HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam );\
protected:
	static CREATE_WND_DATA  s_create_wnd_data;    // 创建窗口时，拦截第一个窗口消息，将HWND->this
	static long CALLBACK ThunkWndProc( HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam );
	long  StartProc( HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam, bool bWindowOrDialog );
	long  WndProc( unsigned int uMsg, WPARAM wParam, LPARAM lParam );
	long  DefWindowProc( unsigned int uMsg, WPARAM wParam, LPARAM lParam );
    long  WndProc_GetRetValue(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled, long lRet);

protected:
	long  _OnSetCursor( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    long  _OnNcHitTest( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnEraseBkgnd( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnPaint( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnPostMessage( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnSize( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnCreate( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnClose(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	long  _OnNcDestroy( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnDestroy( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnHandleMouseMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnHandleKeyBoardMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    long  _OnHandleTouchMessage(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	long  _OnSetFocus( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnKillFocus( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    long  _OnMouseActive( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnThemeChange( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnWindowPosChanging( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    long  _OnShowWindow(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	long  _OnSyncWindow( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnGetMinMaxInfo( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnEnterSizeMove( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnExitSizeMove( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	long  _OnGetObject( unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	void      OnEraseBkgnd(IRenderTarget*);
    void      OnGetDesiredSize(SIZE* pSize);

private:
    void  set_hwnd(HWND);

#pragma endregion


public:
	HWND  m_hWnd;               // 窗口句柄

protected:
    CWndProcThunk  m_thunk;          // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
	WNDPROC  m_oldWndProc;           // 该窗口的原始窗口过程

    SyncWindow       m_syncWindow;
    WindowDragDropMgr      m_oDragDropManager;   // 拖拽管理器
	WindowMouseMgr         m_oMouseManager;      // 鼠标消息处理器
    WindowRender           m_oWindowRender;      // 窗口的渲染放在这个对象里面，windowbase不负责渲染

	CWindowMessageHookProxy*  m_pCallbackProxy;

	bool  m_bDoModal;                // 模态窗口数据
	bool  m_bEndModal;
	INT_PTR  m_lDoModalReturn;


	bool  m_bFirsetEraseBkgnd;       // TODO: 解决第一次显示窗口时，会闪烁一下黑色的问题.

    // 机制：如果控件没有配置字体时，默认取窗口的字体，即m_pDefaultFont
    //
    // 1. 当xml中配置了窗口字体时，m_hFont为xml中的字体。
    // 2. 当没有配置字体时，使用窗口字体。
    // 3. 当窗口字体也没时，使用UI Font Res 默认字体
    // 4. 最后则尝试使用system default font.
	IRenderFont*  m_pDefaultFont;    
	
public:
	// 事件
	signal_mc<long>  size_changed;
    signal_mc<bool&> on_close;
};



}

