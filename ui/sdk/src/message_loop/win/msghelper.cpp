#include "msghelper.h"
#include "sdk/include/macro/helper.h"
#include "src/object/message.h"

namespace ui {

//
//	在ui中实现post message（稍后再响应）
//	见CForwardPostMessageWindow
//
#define UI_MSG_POSTMESSAGE (WM_USER + 1)
struct PostData {
  PostData(slot<void()> &&c) : callback(std::forward<slot<void()>>(c)) {}
  slot<void()> callback;
};

LRESULT ForwardPostMessageWindowProc(HWND hWnd, UINT message, WPARAM wParam,
                                     LPARAM lParam) {
  if (UI_MSG_POSTMESSAGE == message) {
    PostData *data = (PostData *)wParam;
    UIASSERT(data);
    data->callback.emit();
    delete data;
    return TRUE;
  } else if (WM_DESTROY == message) {
    // 将剩余未处理完的post消息释放，避免内存泄露
    // Note that PeekMessage always retrieves WM_QUIT messages,
    // no matter which values you specify for wMsgFilterMin and
    // wMsgFilterMax.

    ::MSG msg;
    while (::PeekMessage(&msg, hWnd, UI_MSG_POSTMESSAGE, UI_MSG_POSTMESSAGE,
                         PM_REMOVE)) {
      if (msg.message == UI_MSG_POSTMESSAGE) {
        PostData *data = (PostData *)wParam;
        UIASSERT(data);
        data->callback.emit();
        delete data;
      }
    }
  }

  return ::DefWindowProc(hWnd, message, wParam, lParam);
}
void ForwardPostMessageWindow::Create(MessageLoopPlatformWin *p) {
  m_bind = p;

  const char *class_name = "ForwardMessageWindows";
  WNDCLASSEXA wx = {};
  wx.cbSize = sizeof(WNDCLASSEX);
  wx.lpfnWndProc = ForwardPostMessageWindowProc;
  wx.hInstance = 0;
  wx.lpszClassName = class_name;
  if (RegisterClassExA(&wx)) {
    m_hWnd = CreateWindowExA(0, class_name, "", 0, 0, 0, 0, 0, HWND_MESSAGE,
                             NULL, NULL, NULL);
  }
}
void ForwardPostMessageWindow::Post(slot<void()> &&callback) {
  PostData *data = new PostData(std::forward<slot<void()>>(callback));
  ::PostMessage(m_hWnd, UI_MSG_POSTMESSAGE, (WPARAM)data, 0);
}

WaitForHandle::WaitForHandle(HANDLE h, IWaitForHandleCallback *pCB, long l) {
  m_hHandle = h;
  m_pCallback = pCB;
  m_lParam = l;
}

WaitForHandlesMgr::WaitForHandlesMgr() {
  m_pHandles = nullptr;
  m_nHandleCount = 0;
}
WaitForHandlesMgr::~WaitForHandlesMgr() {
  _MyIter iter = m_list.begin();
  _MyIter iterEnd = m_list.end();
  for (; iter != iterEnd; iter++) {
    delete (*iter);
  }
  m_list.clear();
  m_nHandleCount = 0;

  if (m_pHandles) {
    delete[] m_pHandles;
    m_pHandles = nullptr;
  }
  m_nHandleCount = 0;
}

WaitForHandle *WaitForHandlesMgr::FindHandle(HANDLE h) {
  if (nullptr == h)
    return nullptr;

  _MyIter iter = m_list.begin();
  _MyIter iterEnd = m_list.end();

  for (; iter != iterEnd; iter++)
    if (h == (*iter)->GetHandle())
      return *iter;

  return nullptr;
}

std::list<WaitForHandle *>::iterator
WaitForHandlesMgr::FindHandleIter(HANDLE h) {
  if (nullptr == h)
    return m_list.end();

  _MyIter iter = m_list.begin();
  _MyIter iterEnd = m_list.end();

  for (; iter != iterEnd; iter++)
    if (h == (*iter)->GetHandle())
      return iter;

  return m_list.end();
}
bool WaitForHandlesMgr::AddHandle(HANDLE h, IWaitForHandleCallback *pCB,
                                  long l) {
  if (nullptr == h || nullptr == pCB)
    return false;

  if (FindHandle(h))
    return false;

  WaitForHandle *p = new WaitForHandle(h, pCB, l);
  m_list.push_back(p);

  UpdateHandleArray();
  return true;
}
bool WaitForHandlesMgr::RemoveHandle(HANDLE h) {
  if (nullptr == h)
    return false;

  _MyIter iter = FindHandleIter(h);
  if (iter == m_list.end())
    return false;

  WaitForHandle *p = *iter;
  if (p) {
    delete p;
    p = nullptr;
  }
  m_list.erase(iter);

  UpdateHandleArray();
  return true;
}

bool WaitForHandlesMgr::UpdateHandleArray() {
  if (m_pHandles) {
    delete[] m_pHandles;
    m_pHandles = nullptr;
  }
  m_nHandleCount = (int)m_list.size();

  if (0 == m_nHandleCount)
    return true;

  m_pHandles = new HANDLE[m_nHandleCount];
  _MyIter iter = m_list.begin();
  _MyIter iterEnd = m_list.end();

  for (int i = 0; iter != iterEnd; iter++, i++) {
    m_pHandles[i] = (*iter)->GetHandle();
  }
  return true;
}

void WaitForHandlesMgr::Do(HANDLE h) {
  WaitForHandle *pWaitForHandle = this->FindHandle(h);
  if (nullptr == pWaitForHandle)
    return;

  pWaitForHandle->GetCB()->OnWaitForHandleObjectCallback(
      (long *)h, pWaitForHandle->GetLParam());
}

// 不使用stl::list，这样至少能在自己的PreTranslateMessage响应中调用RemoveMessageFilter操作
BOOL MessageFilterMgr::PreTranslateMessage(::MSG *pMsg) {
  UIListItem<IPreTranslateMessage *> *p = m_list.GetFirst();
  UIListItem<IPreTranslateMessage *> *pNext = nullptr;

  while (p) {
    pNext = p->GetNext();

    if (p->m_data->PreTranslateMessage(pMsg))
      return TRUE;

    p = pNext;
  }

  return FALSE;
}

void MessageFilterMgr::AddMessageFilter(IPreTranslateMessage *p) {
  if (nullptr == p)
    return;

  m_list.Add(p);
}
void MessageFilterMgr::RemoveMessageFilter(IPreTranslateMessage *p) {
  if (nullptr == p)
    return;

  m_list.Remove(p);
}
} // namespace ui