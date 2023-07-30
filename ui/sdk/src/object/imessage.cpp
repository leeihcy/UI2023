#include "interface/imessage.h"
#include "base/uiapi.h"
#include "base/uimsg.h"
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

void IMessage::AddDelayRef(void **pp) {
  if (!m_pImpl) {
    m_pImpl = new Message(this);
  }
  return m_pImpl->AddDelayRef(pp);
}
void IMessage::RemoveDelayRef(void **pp) {
  if (m_pImpl)
    m_pImpl->RemoveDelayRef(pp);
}

void *IMessage::QueryInterface(const Guid &iid) {
  return (void *)UISendMessage(this, UI_MSG_QUERYINTERFACE, (long)&iid);
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