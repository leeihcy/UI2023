#include <string>

#include "include/event.h"
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
  // 监听 capture阶段的事件
void Message::capture(const char* event_name, slot<void(Event*)>&& s) {
  if (!event_name || !event_name[0]) {
    return;
  }
  m_captures[std::string(event_name)].connect(
      std::forward<slot<void(Event *)>>(s));
}

void Message::emit(Event *event, EventPhase phase) {
  const char *type = event->Type();
  if (!type || !type[0]) {
    return;
  }
  if (phase == EventPhase::Capturing) {
    auto iter = m_captures.find(std::string(type));
    if (iter == m_captures.end()) {
      return;
    }
    iter->second.emit(event);
  } else {
    auto iter = m_events.find(std::string(type));
    if (iter == m_events.end()) {
      return;
    }
    iter->second.emit(event);
  }
}

void Message::clear_events() {
  m_events.clear();
  m_captures.clear();
}

} // namespace ui