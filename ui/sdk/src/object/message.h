#ifndef _UI_SDK_SRC_OBJECT_MESSAGE_H_
#define _UI_SDK_SRC_OBJECT_MESSAGE_H_

#include "include/event.h"
#include "include/interface/imessage.h"
#include "include/common/signalslot/signal.h"
#include <map>
#include <string>

namespace ui {

// 消息能力基类
//
// sizeof(Message) == 3 pointer
//
// . virtual table pointer
// . imessage pointer
// . event map pointer
//
class Message {
public:
  Message(IMessage *);
  virtual ~Message() {};

  IMessage *GetIMessage();
  IMeta& Meta();

public:
  //
  // 将消息从对象的继承链，由下往上进行路由处理。
  // 例如：Button->Control->Object->Message
  // 每个对象将接收到onRouteMessage回调
  //
  void RouteMessage(int message);
  void RouteMessage(Msg* msg);
  void onRouteMessage(ui::Msg *msg) {};

public:
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
  struct EventLintener {
    signal<void(Event*)> bubbling;
    signal<void(Event*)> capture;
  };
  // 为了节省内存，默认只占用一个指针，只在有注册事件时才分配堆内存。
  std::unique_ptr<std::map<std::string, EventLintener>> m_event_listener_map;
};

} // namespace ui

#endif