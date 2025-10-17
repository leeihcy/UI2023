#ifndef _UI_IMESSAGE_H_
#define _UI_IMESSAGE_H_
#include <string.h>

#include "sdk/include/interface.h"
#include "sdk/include/interface/imeta.h"
#include "sdk/include/common/uuid/uuid.h"
#include "sdk/include/macro/uidefine.h"
#include "sdk/include/event.h"
#include "sdk/include/common/signalslot/slot.h"
#include "sdk/include/macro/msg.h"

namespace ui {
struct IMessage;
struct IMeta;

class Message;
struct UIAPI IMessage {
  IMessage(eCreateImpl);
  Message *GetImpl();

  void *QueryInterface(const Uuid &iid);
  IMeta *GetMeta(); // 返回值不会为空
  void RouteMessage(int message);
  void RouteMessage(Msg* msg);

  void connect(const char* event, slot<void(Event*)>&&);

protected:
  friend class Message;
  virtual ~IMessage(); // 虚函数1. 保证正确释放整个对象
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