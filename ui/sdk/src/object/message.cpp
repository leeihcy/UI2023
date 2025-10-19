#include <string>

#include "include/macro/helper.h"
#include "include/util/log.h"

#include "message.h"


namespace ui {

Message::Message(IMessage *imessage) {
  m_imessage = imessage;
}
Message::~Message() {
}

IMessage *Message::GetIMessage() {
  return m_imessage;
}

IMeta *Message::GetMeta() { return m_meta; }

void Message::RouteMessage(int message) {
  ui::Msg msg = { .message = message };
  RouteMessage(&msg);
}
void Message::RouteMessage(Msg* msg) {
  m_meta->RouteMessage(m_imessage, msg);
}

void Message::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    m_meta = static_cast<FinalConstructMessage*>(msg)->meta;
    return;
  }
}

void *Message::QueryInterface(const Uuid &iid) {
  void *result = nullptr;
  QueryInterfaceMessage msg;
  msg.uuid = iid;
  msg.pp = &result;

  RouteMessage(&msg);
  return result;
}

void Message::connect(const char *event_name, slot<void(Event *)> &&s) {
  if (!event_name || !event_name[0]) {
    return;
  }
  m_events[std::string(event_name)].connect(
      std::forward<slot<void(Event *)>>(s));
}

void Message::emit(const char *event_name, Event *event) {
  if (!event_name || !event_name[0]) {
    return;
  }
  auto iter = m_events.find(std::string(event_name));
  if (iter == m_events.end()) {
    return;
  }
  iter->second.emit(event);
}

void Message::clear_events() {
  m_events.clear();
}

} // namespace ui