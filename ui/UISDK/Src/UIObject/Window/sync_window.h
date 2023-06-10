#pragma once
#include "Inc\Interface\iwindow.h"

//////////////////////////////////////////////////////////////////////////
//
//                          联合移动的窗口
//
//////////////////////////////////////////////////////////////////////////

// TODO: 
//		1. 窗口对屏幕边缘的粘贴
//		2. 窗口对其它site window的粘贴及解除粘贴
//		3. host 窗口移动对site window的粘贴



//
//  非UI消息，采用::SendMessage发送
//	当主窗口移动时，向SYNC_CUMTOM类型的窗口发送一个消息去同步自己的位置
//		wparam:  HDWP
//		lparam:  RECT* 
//  return:  HWDP
#define UI_WM_SYNC_WINDOWPOSCHANING  (WM_USER+828)

//
//	通过采用拦截WM_WINDOWPOSCHANGING消息的方法，屏蔽系统处理，由自己再去调用一次DeferWindowPos
//
//	优点:
//		1. 简单，只需要处理一个消息
//		2. 能处理各种窗口位置、大小变化的场景
//
//	缺点:
//		1. 处理不好容易产生死循环
//
class SyncWindow
{
public:
    SyncWindow();
    void  SetHwnd(HWND hWnd);
    void  _OnShowWindow(WPARAM wParam, LPARAM lParam);
	void  _OnWindowPosChanging(LPWINDOWPOS lpWndPos, BOOL& bHandled);
	void   OnHostWindowPosChanging(LPWINDOWPOS lpWndPos);
	bool   OnSiteWindowPosChanging(LPWINDOWPOS lpWndPos);
	bool   CheckRectAnchor(const CRect* prcHost, CRect* prcSite, bool bChangeSize, SyncWindowData* pData);
	void   GetRectByWNDPOS(LPWINDOWPOS lpWndPos, CRect* prc);
	HDWP   HandleWindowAnchor(HDWP& hdwp, const CRect& rcSrcWindow, const SyncWindowData& rData);

	void   _OnNcDestroy();
	HWND   GetMyHWND();
	void   _OnEnterSizeMove();
	void   _OnExitSizeMove();
public:
	bool   AddAnchorItem(const SyncWindowData& data);
	bool   RemoveAnchorItem(HWND hWnd);
	bool   ModifyAnchorItem(const SyncWindowData& data);
    void   OnAddAnchorItem(HWND hHostWnd);     // 自己作为跟随者;
    void   OnModifyAnchorItem(HWND hHostWnd);  // 自己作为跟随者;
    void   OnRemoveAnchorItem(HWND hHostWnd);  // 自己作为跟随者;
    
    void   SyncNow();

	// Host Window移动时，同步site Window位置时，给site window的消息
	void   OnHostWindowPosChanging();
	bool   ClearAnchorItem();
	int    FindAnchorItem(HWND hWnd);
    int    GetAvailableHWNDCount();  // 放弃判断是否可见。有时不可见也需要调整，要不然再显示时，位置就不正确了;

	void   HideAllAnchorItem();
    bool   IsSizeMoveIng();

protected:
	vector<SyncWindowData>   m_vecAnchorItems;
    HWND   m_hWnd;
	HWND   m_hHostWnd;                 // 表示自己当前要跟随哪个窗口进行移动。
                                       // 如果为nullptr则表示自己不跟随其它窗口
	bool   m_bSendByDeferWindowPos;    // WM_WINDOWPOSCHANGING消息的发送者
	byte   m_nAnchorMagnetCapability;  // 停靠的磁性能力。当小于多少px时自动粘合
	bool   m_bAnchorToScreenEdge;      // 是否计算停靠到屏幕边缘
	bool   m_bAnchorToOtherSize;       // 是否计算依靠到其它跟随者
	bool   m_bSizeMove;                // 标明当前是否正在拖拽窗口
};

