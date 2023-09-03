#pragma once
#include "src/util/DataStruct/list.h"
#include "include/interface/iuiinterface.h"

namespace ui
{
class Application;
#if 0
// 创建一个隐藏消息窗口，用于实现post ui message
class ForwardPostMessageWindow : public CWindowImpl<ForwardPostMessageWindow>
{
public:
	ForwardPostMessageWindow(Application* pUIApp) { m_pUIApplication = pUIApp; }
	BOOL ProcessWindowMessage(HWND hWnd, unsigned int uMsg, long wParam, long lParam, long& lResult, unsigned int dwMsgMapID = 0);

protected:
	Application*  m_pUIApplication;
};

// 实现消息循环中等待HANDLE的功能
class WaitForHandle
{
public:
	WaitForHandle(HANDLE h, IWaitForHandleCallback* pCB, long l);
	HANDLE  GetHandle() { return m_hHandle; }
	IWaitForHandleCallback* GetCB() { return m_pCallback; }
	long  GetLParam() { return m_lParam; }

protected:
	HANDLE  m_hHandle;
	IWaitForHandleCallback*  m_pCallback;
	long  m_lParam;
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
	bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, long l);
	bool  RemoveHandle(HANDLE h);
	bool  UpdateHandleArray();
	void  Do(HANDLE h);

	HANDLE*  GetHandles() { return m_pHandles; }

protected:
	list<WaitForHandle*>  m_list;
public:
	HANDLE*  m_pHandles;
	unsigned int    m_nHandleCount;  // 因为要在while循环中调用，因此作为public变量直接调用
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
#endif

}

