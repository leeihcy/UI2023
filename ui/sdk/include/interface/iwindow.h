#ifndef _INCLUDED_IWINDOW_
#define _INCLUDED_IWINDOW_

#include "../base/uidefine.h"
#include "../common/signalslot/signal.h"
#include "imessage.h"
#include <SkCanvas.h>
#include <SkSurface.h>
// #include "irenderlayer.h"
// #include "ipanel.h"

namespace ui
{
#if 0
struct IWindowMouseMgr;
struct IWindowKeyboardMgr;

//
//  ��UI��Ϣ������::SendMessage����
//  ����һ��ͬ���ƶ������¼�(��ӡ��޸ġ�ɾ��)
//		wparam:  SYNC_WINDOW_EVENT_TYPE
//		lparam:  ����鿴SYNC_WINDOW_EVENT_TYPE����
#define UI_WM_SYNC_WINDOW  (WM_USER+827)

#define ANCHOR_NONE        0
#define ANCHOR_LEFT        0x0001
#define ANCHOR_RIGHT       0x0002
#define ANCHOR_TOP         0x0004
#define ANCHOR_BOTTOM      0x0008
#define ANCHOR_OUT_LEFT    0x0010
#define ANCHOR_OUT_RIGHT   0x0020
#define ANCHOR_OUT_TOP     0x0040
#define ANCHOR_OUT_BOTTOM  0x0080
#define ANCHOR_CUSTOM      0x0100   // ����UI_WM_SYNC_WINDOWPOSCHANGING��Ϣ�����ڣ��ɴ������о�������ƶ��Լ�
#define ANCHOR_CLIENTREGION  0x200  // ��host�Ŀͻ������룬����host�Ǹ���ͨ�߿򴰿�


enum SYNC_WINDOW_EVENT_TYPE
{
    ADD_SYNC_WINDOW,            // ֪ͨHOST, ����һ��������.LPARAM: SyncWindowData*
    MODIFY_SYNC_WINDOW,         // ֪ͨHOST, �޸�һ����������Ϣ.LPARAM: SyncWindowData*
    REMOVE_SYNC_WINDOW,         // ֪ͨHOST, �Ƴ�һ��������.LPARAM: SyncWindowData*

    ADD_SYNC_WINDOW_RESULT,     // ֪ͨSITE����ӳɹ�.LPARAM: host HWND
    MODIFY_SYNC_WINDOW_RESULT,  // ֪ͨSITE���޸ĳɹ�.LPARAM: host HWND
    REMOVE_SYNC_WINDOW_RESULT,  // ֪ͨSITE��ɾ���ɹ�.LPARAM: host HWND

    HOST_WINDOW_DESTROYED,      // Ҫ�����HOST�����ˣ�����Լ���HOST���. LPARAM: ��
    HOST_WINDOW_POSCHANGING,    // ���͸�SITE���ڣ�������һ����Ӧ��WindowPosChaning��Ϣ����ΪHOST�ƶ�������

    SYNC_NOW,                   // ���͸�host������ͬ��һ�Σ��細�ڸմ������
};

struct  AnchorData
{
    int  xOffset;
    int  yOffset;

    union{
        int xOffset2;
        int Width;         // -1 ��ʾ���޸Ĵ��ڿ��
    };
    union{
        int yOffset2;
        int Height;       // -1 ��ʾ���޸Ĵ��ڸ߶�
    };

    AnchorData()
    {
        xOffset = yOffset = xOffset2 = yOffset2 = 0;
    }
};

#define SWDS_MASK_ANCHORTYPE    0x1
#define SWDS_MASK_ANCHORDATA    0x2
#define SWDS_MASK_ANCHORON      0x4
#define SWDS_MASK_SYNC_VISIBLE  0x8
#define SWDS_MASK_ALL           0xFFFF

struct  SyncWindowData
{
    HWND         m_hWnd;           
    UINT         m_nMask;
    UINT         m_nAnchorType;   
    AnchorData   m_rcAnchorData;  // ��m_nAnchorType��Ӧλ��ֵʱ��m_rcAnchorData�ĸ�λ��Ч
    bool         m_bAnchorOn;     // �����Ƿ������á�������ʱ�������Ƴ�HOST���ڣ������и��棬Ȼ��������HOST�������ٴο������档
	bool         m_bSyncVisible;  // �Ƿ�ͬ������

    SyncWindowData()
    {
        m_hWnd = nullptr;
        m_nAnchorType = ANCHOR_NONE;
        m_bAnchorOn = true;
		m_bSyncVisible = false;

        m_nMask = 0;
    }
};

// window style
typedef struct tagWindowStyle
{
	bool  destroyed : 1;       // ��ʾ�ô����Ѿ���������(WM_NCDESTROY)�����ڴ���OnFinalMessage
	bool  attach : 1;          // ��ʾ�ô�����attach�ģ��������������ⲿ������
	bool  setcreaterect : 1;   // ��������ʱָ���˴��ڴ�С�������ٽ��д��ڲ�����
	bool  dialog_noresize : 1; // ���ڽ��win7����Dialog��ʾ��С��GetWindowRect��һ�µ�����
	bool  hard_composite: 1;   // ������ʹ��Ӳ���ϳ� 
}WindowStyle;

// �����ⲿҵ��ʵ���ߴ�������Ϣ��
// �ⲿ����Ҫ���IWindowBase������ֻ��Ҫʵ��IWindowMessageCallback�ӿڼ��ɡ�
struct IWindowDelegate
{
    virtual BOOL  OnWindowMessage(UINT, WPARAM, LPARAM, LRESULT& lResult) { return FALSE; }
    virtual BOOL  OnWindowUIMessage(UIMSG* pMsg) { return FALSE; }
    virtual void  OnWindowInitialize() {}
	virtual void  DoBindPlz(bool bind) {}  // �ڻ���ʱ����֪ͨһ�Σ���Ϊ�п����������Ŀؼ�
    virtual void  OnWindowClose(bool& bCanClose) { }
    virtual void  OnWindowDestroy() {};
};

class WindowBase;
struct UIAPI_UUID(1C7CED21-3CF6-49C9-9E52-72522C8A1CF6) IWindowBase
 : public IPanel
{
    HWND  GetHWND();
    IWindowRender*  GetWindowRender();
	ISkinRes*  GetSkinRes();

    BOOL  IsChildWindow();
	BOOL  IsWindowVisible();
    void  ShowWindow();
    void  HideWindow();
    bool  IsDoModal();
    void  CenterWindow(HWND hWndCenter = nullptr);
    void  CenterWindow(HMONITOR hMonitor = nullptr);
	void  GetWindowNormalRect(LPRECT prc);
	void  SetWindowNormalRect(LPCRECT prc);
	void  UpdateDesktopLayout();

    void  SetFocusObject(IObject* pObj);
	void  SetPressObject(IObject* pObj);
    IObject*  GetHoverObject();
    IObject*  GetPressObject();
    IObject*  GetObjectByPos(IObject* pObjParent, POINT* pt, bool bSkinBuilderInvoke=false);

    bool  Create(LPCTSTR szID, HWND hWndParent=nullptr, RECT* prc=nullptr, long lStyle = 0, long lExStyle = 0);
    bool  Attach(HWND hWnd, LPCTSTR szID);
    void  Detach();
	INT_PTR  DoModal(LPCTSTR szID, HWND hWndParent, bool canResize);
	INT_PTR  DoModal(HINSTANCE hResInst, UINT nResID, LPCTSTR szID, HWND hWndParent);
    HWND  DoModeless(LPCTSTR szID, HWND hWndParent, bool canResize);
    HWND  DoModeless(HINSTANCE hResInst, UINT nResID, LPCTSTR szID, HWND hWndParent);
    void  EndDialog(INT_PTR);
	void  DestroyWindow();
	void  ChangeSkinLayout(LPCTSTR szLayoutId);

    void  SaveMemBitmap(TCHAR* szFile);
	void  AlphaBlendMemBitmap(HDC hDC, RECT* prc, int alpha);
	void  BitBltMemBitmap(HDC hDC, RECT* prc);
    void  EnableDwmTransition(bool b);
	void  EnableGpuComposite(bool b);
	bool  IsGpuComposite();
	void  DirectComposite();
	void  SetWindowMessageCallback(IWindowDelegate*);

    void  CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow );
    void  CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient );

//     bool  AddAnchorItem(const SyncWindowData& data);
//     void  HideAllAnchorItem();

    HRESULT  SetDroppable(bool b);
	bool  IsSizeMoveIng();

	signal_mc<long>&  SizeChangedEvent();
    signal_mc<bool&>&  OnCloseEvent();  // void  OnWindowClose(bool& bCanClose);
    UI_DECLARE_INTERFACE(WindowBase);
};

#endif


class Window;
struct UIAPI_UUID(5C36801E-5929-4512-A998-F9719DCC6903) IWindow : public IMessage
{
  void Create(const Rect &rect);
  void SetTitle(const char* title);
  void Show();

  ui::signal<void()> &DestroySignal();
  ui::signal<void(SkCanvas&)> &PaintSignal();
  
  UI_DECLARE_INTERFACE(Window)
};

#if 0
extern "C"
void UIAPI GetWindowNormalRect(HWND hWnd, LPRECT prc);
#endif
}

#endif  // _INCLUDED_IWINDOW_