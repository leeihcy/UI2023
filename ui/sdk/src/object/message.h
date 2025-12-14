#ifndef _UI_SDK_SRC_OBJECT_MESSAGE_H_
#define _UI_SDK_SRC_OBJECT_MESSAGE_H_

#include "include/event.h"
#include "include/interface/imessage.h"
#include "include/common/signalslot/signal.h"
#include <map>
#include <string>

namespace ui {

// 消息能力基类
class Message {
public:
  Message(IMessage *);
  virtual ~Message() {};

  IMessage *GetIMessage();
  IMeta& Meta();

public:
  void RouteMessage(int message);
  void RouteMessage(Msg* msg);
  void onRouteMessage(ui::Msg *msg) {};
  void* QueryInterface(const Uuid &iid);

public:
  //
  // 参看 EventDispatcher 实现原理
  //
  // 监听 attarge/bubbling 阶段的事件
  void Connect(const char* event_name, slot<void(Event*)>&& s);
  // 监听 capture 阶段的事件
  void Capture(const char* event, slot<void(Event*)>&&);
  // 发送事件给对应listener
  void Emit(Event* event, EventPhase phase = EventPhase::AtTarget);
  void ClearEvents();

protected:
  IMessage *m_imessage = nullptr;

  // 当前对象监听了哪些事件
  std::map<std::string, signal<void(Event*)>> m_events;
  std::map<std::string, signal<void(Event*)>> m_captures;
};

} // namespace ui

#endif