#ifndef _UI_SDK_SRC_OBJECT_MESSAGE_H_
#define _UI_SDK_SRC_OBJECT_MESSAGE_H_

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

  void connect(const char* event_name, slot<void(Event*)>&& s);
  void emit(const char* event_name, Event* event);
  void clear_events();

  IMeta *GetMeta();
  void RouteMessage(int message);
  void RouteMessage(Msg* msg);
  void onRouteMessage(ui::Msg *msg);

protected:
  // 事件定义
  std::map<std::string, signal<void(Event*)>> m_events;

  // 对象的一些静态属性，仅保存指针，通常这是一个static对象地址。
  IMeta *m_meta = nullptr; 

  IMessage *m_imessage = nullptr;
};

} // namespace ui

#endif