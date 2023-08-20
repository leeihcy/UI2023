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
  bool ProcessMessage(UIMSG *pMsg, int nMsgMapID = 0, bool bDoHook = false);
  // void Release();

  void connect(const char* event, slot<void(Event*)>&&);

  IMeta *GetMeta(); // 返回值不会为空

  // long UIPostMessage(IApplication *pUIApp, UIMSG *pMsg, int nMsgMapID = 0);
  static long SendMessage(UIMSG *pMsg, int nMsgMapID = 0,
                          bool *pbHandled = nullptr);
  long SendMessage(unsigned int message, long wParam = 0, long lParam = 0,
                   unsigned int nCode = 0, IMessage *pMsgFrom = nullptr,
                   int nMsgMapID = 0, bool *pbHandled = nullptr);

  bool IsMsgHandled() const;
  void SetMsgHandled(bool b);
  UIMSG *GetCurMsg();
  void SetCurMsg(UIMSG *p);
  bool DoHook(UIMSG *pMsg, int nMsgMapID);

protected:
  friend class Message;
  virtual ~IMessage(); // 虚函数1. 保证正确释放整个对象
private:
  virtual bool virtualProcessMessage(UIMSG *pMsg, int nMsgMapID,
                                     bool bDoHook); // 虚函数2. 消息处理
  // virtual void virtual_delete_this(); // 由UIObjCreator负责实现

public:
  Message *GetImpl();
  void ClearNotify();
  void SetNotify(IMessage *pObj, int nMsgMapID);
  long DoNotify(UIMSG *pMsg);
  IMessage *GetNotifyObj();
  void CopyNotifyTo(IMessage *pObjCopyTo);

  void AddHook(IMessage *pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
  void RemoveHook(IMessage *pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
  void RemoveHook(IMessage *pObj);
  void ClearHook();

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

  bool IsMsgHandled() const;
  void SetMsgHandled(bool b);
  UIMSG *GetCurMsg();
  void SetCurMsg(UIMSG *p);
  bool DoHook(UIMSG *pMsg, int nMsgMapID);

  virtual bool virtualProcessMessage(ui::UIMSG *pMsg, int nMsgMapID = 0,
                                     bool bDoHook = false) {
    return false;
  }

protected:
  Message *m_pImpl;
};

} // namespace ui

#endif // _UI_IMESSAGE_H_