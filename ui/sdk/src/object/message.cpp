#include "message.h"

#include "include/event.h"
#include "include/macro/helper.h"
#include "include/util/log.h"

namespace ui {

Message::Message(IMessage *imessage) {
  m_imessage = imessage;
}

IMessage *Message::GetIMessage() {
  return m_imessage;
}

IMeta& Message::Meta() { 
  return m_imessage->Meta(); 
}

void Message::RouteMessage(int message) {
  ui::Msg msg = { .message = message };
  RouteMessage(&msg);
}
void Message::RouteMessage(Msg* msg) {
  Meta().RouteMessage(m_imessage, msg);
}

void *Message::QueryInterface(const Uuid &iid) {
  void *result = nullptr;
  QueryInterfaceMessage msg;
  msg.uuid = iid;
  msg.pp = &result;

  RouteMessage(&msg);
  return result;
}

void Message::Connect(const char *event_name, slot<void(Event *)> &&s) {
  if (!event_name || !event_name[0]) {
    return;
  }
  if (!m_event_listener_map) {
    m_event_listener_map = std::make_unique<std::map<std::string, EventLintener>>();
  }
  
  EventLintener& listener = (*m_event_listener_map)[std::string(event_name)];
  listener.bubbling.connect(std::forward<slot<void(Event *)>>(s));
}

  // 监听 capture阶段的事件
void Message::Capture(const char* event_name, slot<void(Event*)>&& s) {
  if (!event_name || !event_name[0]) {
    return;
  }
  if (!m_event_listener_map) {
    m_event_listener_map = std::make_unique<std::map<std::string, EventLintener>>();
  }
  EventLintener& listener = (*m_event_listener_map)[std::string(event_name)];
  listener.capture.connect(std::forward<slot<void(Event *)>>(s));
}

void Message::Emit(Event *event, EventPhase phase) {
  const char *type = event->Type();
  if (!type || !type[0]) {
    return;
  }
  if (!m_event_listener_map) {
    return;
  }
  auto iter = m_event_listener_map->find(std::string(type));
  if (iter == m_event_listener_map->end()) {
    return;
  }

  if (phase == EventPhase::Capturing) {
    iter->second.capture.emit(event);
  } else {
    iter->second.bubbling.emit(event);
  }
}

void Message::ClearEvents() {
  m_event_listener_map.reset();
}

} // namespace ui