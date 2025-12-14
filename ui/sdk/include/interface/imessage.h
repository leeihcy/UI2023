#ifndef _UI_IMESSAGE_H_
#define _UI_IMESSAGE_H_

#include "sdk/include/interface.h"
#include "sdk/include/interface/imeta.h"
#include "sdk/include/common/signalslot/slot.h"
#include "sdk/include/macro/msg.h"

namespace ui {
struct IMessage;
struct IMeta;
class Event;

class Message;
struct UIAPI IMessage {
  IMessage(eCreateImpl);
  Message *GetImpl();

  void *QueryInterface(const Uuid &iid);
  void RouteMessage(int message);
  void RouteMessage(Msg* msg);

  // 监听 attarge/bubbling阶段的事件
  void Connect(const char* event, slot<void(Event*)>&&);
  // 监听 capture阶段的事件
  void Capture(const char* event, slot<void(Event*)>&&);

protected:
  friend class Message;

  // 虚函数1. 保证正确释放整个对象
  virtual ~IMessage(); 

  // 虚函数2. 获取对象静态数据，
  // . 避免在Message对象中再保存一个IMeta *m_meta成员变量，节省内存。
  // . 将其它虚函数可以放到IMeta中，保持Object对象模型干净。
  virtual IMeta& Meta() = 0; 

protected:
  Message *m_pImpl;
};

// 由外部的类继承，用于支持消息映射宏定义
class UIAPI MessageProxy {
public:
  MessageProxy(IMessage *p);
  virtual ~MessageProxy();

protected:
  Message *m_pImpl;
};

} // namespace ui

#endif // _UI_IMESSAGE_H_