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
  IMessage(E_BOOL_CREATE_IMPL);

#if 0 // 废弃，使用RouteMessage代替。 
  bool ProcessMessage(UIMSG *pMsg, int nMsgMapID = 0, bool bDoHook = false);
#endif
  // void Release();

  void connect(const char* event, slot<void(Event*)>&&);

  IMeta *GetMeta(); // 返回值不会为空
  void RouteMessage(int message);
  void RouteMessage(Msg* msg);

#if 0 // 废弃，使用RouteMessage代替。
  // llong UIPostMessage(IApplication *pUIApp, UIMSG *pMsg, int nMsgMapID = 0);
  static llong SendMessage(UIMSG *pMsg, int nMsgMapID = 0,
                          bool *pbHandled = nullptr);
  llong SendMessage(unsigned int message, llong wParam = 0, llong lParam = 0,
                   unsigned int nCode = 0, IMessage *pMsgFrom = nullptr,
                   int nMsgMapID = 0, bool *pbHandled = nullptr);


  bool IsMsgHandled() const;
  void SetMsgHandled(bool b);
  UIMSG *GetCurMsg();
  void SetCurMsg(UIMSG *p);
  bool DoHook(UIMSG *pMsg, int nMsgMapID);
#endif

protected:
  friend class Message;
  virtual ~IMessage(); // 虚函数1. 保证正确释放整个对象
private:

#if 0 // 废弃，使用RouteMessage代替。
  virtual bool virtualProcessMessage(UIMSG *pMsg, int nMsgMapID,
                                     bool bDoHook); // 虚函数2. 消息处理
  // virtual void virtual_delete_this(); // 由UIObjCreator负责实现
#endif

public:
  Message *GetImpl();

#if 0 // 废弃，使用RouteMessage代替。  
  void ClearNotify();
  void SetNotify(IMessage *pObj, int nMsgMapID);
  llong DoNotify(UIMSG *pMsg);
  IMessage *GetNotifyObj();
  void CopyNotifyTo(IMessage *pObjCopyTo);

  void AddHook(IMessage *pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
  void RemoveHook(IMessage *pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
  void RemoveHook(IMessage *pObj);
  void ClearHook();
#endif
  // void AddDelayRef(void **pp);
  // void RemoveDelayRef(void **pp);

  void *QueryInterface(const Uuid &iid);

protected:
  Message *m_pImpl;
};

// 由外部的类继承，用于支持消息映射宏定义
class UIAPI MessageProxy {
public:
  MessageProxy(IMessage *p);
  virtual ~MessageProxy();

#if 0 // 废弃，使用RouteMessage代替。  
  bool IsMsgHandled() const;
  void SetMsgHandled(bool b);
  UIMSG *GetCurMsg();
  void SetCurMsg(UIMSG *p);
  bool DoHook(UIMSG *pMsg, int nMsgMapID);

  virtual bool virtualProcessMessage(ui::UIMSG *pMsg, int nMsgMapID = 0,
                                     bool bDoHook = false) {
    return false;
  }
#endif

protected:
  Message *m_pImpl;
};

} // namespace ui

#endif // _UI_IMESSAGE_H_