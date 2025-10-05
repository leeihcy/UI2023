#ifndef _UI_SDK_SRC_MESSAGELOOP_WIN_MSGHELPER_H_
#define _UI_SDK_SRC_MESSAGELOOP_WIN_MSGHELPER_H_
#include "include/interface/iuiinterface.h"
#include "src/util/DataStruct/list.h"
#include "src/util/windows.h"
#include <list>

namespace ui {
class Application;
class MessageLoopPlatformWin;

// 创建一个隐藏消息窗口，用于实现post ui message
class ForwardPostMessageWindow{
public:
  void Create(MessageLoopPlatformWin* p);
  void Post(slot<void()>&& callback);
  HWND GetHWnd() { return m_hWnd; }

public:
  void onTimer(UINT_PTR id);
protected:
  MessageLoopPlatformWin *m_bind; // raw ptr
  HWND m_hWnd;
};

class MessageFilterMgr : public IMessageFilterMgr {
public:
  ~MessageFilterMgr(){};

  virtual void AddMessageFilter(IPreTranslateMessage *p);
  virtual void RemoveMessageFilter(IPreTranslateMessage *p);

public:
  BOOL PreTranslateMessage(::MSG *pMsg);

protected:
  UIList<IPreTranslateMessage *> m_list;
};

struct IWaitForHandlesMgr {
  virtual bool AddHandle(HANDLE h, IWaitForHandleCallback *pCB, long l) = 0;
  virtual bool RemoveHandle(HANDLE h) = 0;
};

// 实现消息循环中等待HANDLE的功能
// class WaitForHandle {
// public:
//   WaitForHandle(HANDLE h, IWaitForHandleCallback *pCB, long l);
//   HANDLE GetHandle() { return m_hHandle; }
//   IWaitForHandleCallback *GetCB() { return m_pCallback; }
//   long GetLParam() { return m_lParam; }

// protected:
//   HANDLE m_hHandle;
//   IWaitForHandleCallback *m_pCallback;
//   long m_lParam;
// };

// class WaitForHandlesMgr : public IWaitForHandlesMgr {
// public:
//   WaitForHandlesMgr();
//   ~WaitForHandlesMgr();

//   typedef std::list<WaitForHandle *> _MyList;
//   typedef std::list<WaitForHandle *>::iterator _MyIter;

//   WaitForHandle *FindHandle(HANDLE h);
//   _MyIter FindHandleIter(HANDLE h);
//   bool AddHandle(HANDLE h, IWaitForHandleCallback *pCB, long l);
//   bool RemoveHandle(HANDLE h);
//   bool UpdateHandleArray();
//   void Do(HANDLE h);

//   HANDLE *GetHandles() { return m_pHandles; }

// protected:
//   std::list<WaitForHandle *> m_list;

// public:
//   HANDLE *m_pHandles;
//   unsigned int
//       m_nHandleCount; // 因为要在while循环中调用，因此作为public变量直接调用
// };

} // namespace ui

#endif