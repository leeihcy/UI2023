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
  virtual ~Message();

  IMessage *GetIMessage();
  void* QueryInterface(const Uuid &iid);

  // 监听 attarge/bubbling阶段的事件
  void connect(const char* event_name, slot<void(Event*)>&& s);
  // 监听 capture阶段的事件
  void capture(const char* event, slot<void(Event*)>&&);
  // 发送事件给对应listener
  void emit(Event* event, EventPhase phase = EventPhase::AtTarget);
  void clear_events();

  IMeta *GetMeta();
  void RouteMessage(int message);
  void RouteMessage(Msg* msg);
  void onRouteMessage(ui::Msg *msg);

protected:
  // 当前对象监听了哪些事件
  std::map<std::string, signal<void(Event*)>> m_events;
  std::map<std::string, signal<void(Event*)>> m_captures;

  // 对象的一些静态属性，仅保存指针，通常这是一个static对象地址。
  IMeta *m_meta = nullptr; 

  IMessage *m_imessage = nullptr;
};

} // namespace ui

#endif