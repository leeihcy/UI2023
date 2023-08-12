#include "include/interface/imessage.h"
#include "include/uiapi.h"
#include "include/macro/uimsg.h"
#include "include/util/log.h"
#include "message.h"

namespace ui {

IMessage::IMessage(E_BOOL_CREATE_IMPL b) {
  if (b)
    m_pImpl = new Message(this);
  else
    m_pImpl = nullptr;
}

IMessage::~IMessage() { SAFE_DELETE(m_pImpl); }

Message *IMessage::GetImpl() { return m_pImpl; }

bool IMessage::ProcessMessage(UIMSG *pMsg, int nMsgMapID, bool bDoHook) {
  ui::UIMSG *pOldMsg = nullptr;
  if (m_pImpl)
    pOldMsg = m_pImpl->GetCurMsg();

  bool bRet = virtualProcessMessage(pMsg, nMsgMapID, bDoHook);

  if (m_pImpl)
    m_pImpl->SetCurMsg(pOldMsg);

  return bRet;
}

//
//	return
//		利用pMsg->lRet作为返回值。
//			事件响应函数的结果返回值作为pMsg->lRet;
//			如果事件响应函数没有返回值，pMsg->lRet默认为0
//	remark
//		想要知道这个消息有没有被处理，可调用IsMsgHandled()
//
//  注：对于系统的SendMessage，它的nMsg有范围限制的：
//     if(Msg&0xFFFE0000)// Msgs (>0xFFFF) are reserved by the system
//     {
//         User32._UserSetLaseError(0x57); //0x57 The parameter is incorrect.
//         return 0;
//     }
/*static*/
long IMessage::SendMessage(UIMSG *pMsg, int nMsgMapID, bool *pbHandled) {
  UIASSERT(pMsg != nullptr);
  if (nullptr == pMsg) {
    UI_LOG_ERROR("UISendMessage, pMsg==nullptr! ");
    return -1;
  }

  UIASSERT(pMsg->pMsgTo != nullptr);
  //	UIASSERT(pMsg->message != 0);   // 有可能是WM_NULL

  if (nullptr == pMsg->pMsgTo) {
    UI_LOG_ERROR("UISendMessage, pMessageTo==nullptr!  message=%d",
                 pMsg->message);
    return -1;
  }

  bool bRet = pMsg->pMsgTo->ProcessMessage(pMsg, nMsgMapID, true);
  if (pbHandled) {
    *pbHandled = bRet;
  }

  return pMsg->lRet;
}

long IMessage::SendMessage(uint message, long wParam, long lParam,
                   uint nCode, IMessage *pMsgFrom, int nMsgMapID,
                   bool *pbHandled) {
  UIMSG msg;
  msg.pMsgFrom = pMsgFrom;
  msg.pMsgTo = this;

  msg.message = message;
  msg.nCode = nCode;
  msg.wParam = wParam;
  msg.lParam = lParam;

  bool bRet = ProcessMessage(&msg, nMsgMapID, true);
  if (pbHandled) {
    *pbHandled = bRet;
  }

  return msg.lRet;
}

// long IMessage::SendMessage(uint message, long wParam, long lParam) {
//   return SendMessage(message, wParam, lParam, 0,
//                        nullptr, 0, nullptr);
// }

// 不支持跨线程调用
// long UIPostMessage(HWND hForwardMsgWnd, UIMSG *pMsg, int nMsgMapID) {
//   if (!pMsg->pMsgTo)
//     return 0;

//   UIMSG *pCloneMsg = new UIMSG;
//   memcpy(pCloneMsg, pMsg, sizeof(UIMSG));

//   pMsg->pMsgTo->AddDelayRef((void **)&pCloneMsg->pMsgTo);

//   // -->ForwardPostMessageWindow::ProcessWindowMessage
// #if defined(OS_WIN)
//   ::PostMessage(hForwardMsgWnd, UI_MSG_POSTMESSAGE, (long)pCloneMsg,
//                 (long)nMsgMapID);
// #else
//   UIASSERT(0);
// #endif

//   return 0;
// }

// 由于pMsg.pObjTo是一个Message类型，无法获取pUIApp指针，只能再增加一个参数
// long UIPostMessage(IApplication *pUIApp, UIMSG *pMsg, int nMsgMapID) {
//   if (nullptr == pUIApp)
//     return 0;
// #if defined(OS_WIN)
//   HWND hDestWnd = pUIApp->GetForwardPostMessageWnd();
//   if (nullptr == hDestWnd)
//     return 0;

//   return UIPostMessage(hDestWnd, pMsg, nMsgMapID);
// #else
//   // NOT IMPL
//   UIASSERT(false);
//   return 0;
// #endif
// }


void IMessage::Release() { this->virtual_delete_this(); }

bool IMessage::IsMsgHandled() const { return m_pImpl->IsMsgHandled(); }
void IMessage::SetMsgHandled(bool b) { m_pImpl->SetMsgHandled(b); }
UIMSG *IMessage::GetCurMsg() { return m_pImpl->GetCurMsg(); }
void IMessage::SetCurMsg(UIMSG *p) { m_pImpl->SetCurMsg(p); }
bool IMessage::DoHook(UIMSG *pMsg, int nMsgMapID) {
  return m_pImpl->DoHook(pMsg, nMsgMapID);
}

bool IMessage::virtualProcessMessage(UIMSG *pMsg, int nMsgMapID, bool bDoHook) {
  return false;
}

void IMessage::virtual_delete_this() { delete this; }

// bool  IMessage::DoHook(UIMSG* pMsg, int nMsgMapID)
// {
//     return m_pImpl->DoHook(pMsg, nMsgMapID);
// }

void IMessage::ClearNotify() { return m_pImpl->ClearNotify(); }
void IMessage::SetNotify(IMessage *pObj, int nMsgMapID) {
  return m_pImpl->SetNotify(pObj, nMsgMapID);
}
void IMessage::CopyNotifyTo(IMessage *pObjCopyTo) {
  if (nullptr == pObjCopyTo)
    return;

  return m_pImpl->CopyNotifyTo(pObjCopyTo);
}
long IMessage::DoNotify(UIMSG *pMsg) { return m_pImpl->DoNotify(pMsg); }

IMessage *IMessage::GetNotifyObj() { return m_pImpl->GetNotifyObj(); }

void IMessage::AddHook(IMessage *pObj, int nMsgMapIDToHook,
                       int nMsgMapIDToNotify) {
  if (nullptr == pObj)
    return;

  return m_pImpl->AddHook(pObj /*->GetMessageImpl()*/, nMsgMapIDToHook,
                          nMsgMapIDToNotify);
}
void IMessage::RemoveHook(IMessage *pObj, int nMsgMapIDToHook,
                          int nMsgMapIDToNotify) {
  if (nullptr == pObj)
    return;

  return m_pImpl->RemoveHook(pObj /*->GetMessageImpl()*/, nMsgMapIDToHook,
                             nMsgMapIDToNotify);
}
void IMessage::RemoveHook(IMessage *pObj) {
  if (!pObj)
    return;

  return m_pImpl->RemoveHook(pObj /*->GetMessageImpl()*/);
}
void IMessage::ClearHook() { return m_pImpl->ClearHook(); }

// void IMessage::AddDelayRef(void **pp) {
//   if (!m_pImpl) {
//     m_pImpl = new Message(this);
//   }
//   return m_pImpl->AddDelayRef(pp);
// }
// void IMessage::RemoveDelayRef(void **pp) {
//   if (m_pImpl)
//     m_pImpl->RemoveDelayRef(pp);
// }

void *IMessage::QueryInterface(const Uuid &iid) {
  return (void *)SendMessage(UI_MSG_QUERYINTERFACE, (long)&iid);
}

MessageProxy::MessageProxy(IMessage *p) : m_pImpl(p->GetImpl()) {}
MessageProxy::~MessageProxy() {}

bool MessageProxy::IsMsgHandled() const { return m_pImpl->IsMsgHandled(); }
void MessageProxy::SetMsgHandled(bool b) { return m_pImpl->SetMsgHandled(b); }
UIMSG *MessageProxy::GetCurMsg() { return m_pImpl->GetCurMsg(); }
void MessageProxy::SetCurMsg(UIMSG *p) { m_pImpl->SetCurMsg(p); }
bool MessageProxy::DoHook(UIMSG *pMsg, int nMsgMapID) {
  return m_pImpl->DoHook(pMsg, nMsgMapID);
}

} // namespace ui