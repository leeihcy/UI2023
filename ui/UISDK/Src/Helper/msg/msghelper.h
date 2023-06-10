#pragma once
#include "Src\Util\DataStruct\list.h"
#include "Inc\Interface\iuiinterface.h"

namespace UI
{
class UIApplication;
// 创建一个隐藏消息窗口，用于实现post ui message
class ForwardPostMessageWindow : public CWindowImpl<ForwardPostMessageWindow>
{
public:
	ForwardPostMessageWindow(UIApplication* pUIApp) { m_pUIApplication = pUIApp; }
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);

protected:
	UIApplication*  m_pUIApplication;
};

// 实现消息循环中等待HANDLE的功能
class WaitForHandle
{
public:
	WaitForHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l);
	HANDLE  GetHandle() { return m_hHandle; }
	IWaitForHandleCallback* GetCB() { return m_pCallback; }
	LPARAM  GetLParam() { return m_lParam; }

protected:
	HANDLE  m_hHandle;
	IWaitForHandleCallback*  m_pCallback;
	LPARAM  m_lParam;
};

 
class WaitForHandlesMgr : public IWaitForHandlesMgr
{
public:
	WaitForHandlesMgr();
	~WaitForHandlesMgr();

	typedef list<WaitForHandle*> _MyList;
	typedef list<WaitForHandle*>::iterator _MyIter;

	WaitForHandle* FindHandle(HANDLE h);
	_MyIter FindHandleIter(HANDLE h);
	bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l);
	bool  RemoveHandle(HANDLE h);
	bool  UpdateHandleArray();
	void  Do(HANDLE h);

	HANDLE*  GetHandles() { return m_pHandles; }

protected:
	list<WaitForHandle*>  m_list;
public:
	HANDLE*  m_pHandles;
	DWORD    m_nHandleCount;  // 因为要在while循环中调用，因此作为public变量直接调用
};

class MessageFilterMgr : public IMessageFilterMgr
{
public:
    ~MessageFilterMgr() {};

	virtual void  AddMessageFilter(IPreTranslateMessage* p);
	virtual void  RemoveMessageFilter(IPreTranslateMessage* p);

public:
	BOOL  PreTranslateMessage(MSG* pMsg);
	
protected:
    UIList<IPreTranslateMessage*> m_list;
};

}

