#include "include/interface/imessage.h"
#include "include/macro/helper.h"
#include "include/macro/uimsg.h"
#include "include/uiapi.h"
#include "message.h"

namespace ui {

IMessage::IMessage(eCreateImpl b) {
  if (b == eCreateImpl::True)
    m_pImpl = new Message(this);
  else
    m_pImpl = nullptr;
}

IMessage::~IMessage() { SAFE_DELETE(m_pImpl); }

Message *IMessage::GetImpl() { return m_pImpl; }

void IMessage::connect(const char *event, slot<void(Event *)> &&s) {
  m_pImpl->connect(event, std::forward<slot<void(Event *)>>(s));
}

IMeta *IMessage::GetMeta() { return m_pImpl->GetMeta(); }

void IMessage::RouteMessage(int message) { m_pImpl->RouteMessage(message); }
void IMessage::RouteMessage(Msg *msg) { m_pImpl->RouteMessage(msg); }

void *IMessage::QueryInterface(const Uuid &iid) {
  void *result = nullptr;
  QueryInterfaceMessage msg;
  msg.uuid = iid;
  msg.pp = &result;

  RouteMessage(&msg);
  return result;
}

MessageProxy::MessageProxy(IMessage *p) : m_pImpl(p->GetImpl()) {}
MessageProxy::~MessageProxy() {}

} // namespace ui