#ifndef _UI_SDK_SRC_OBJECT_MESSAGE_H_
#define _UI_SDK_SRC_OBJECT_MESSAGE_H_

#include "include/interface/imessage.h"
#include <list>

namespace ui {
//
// 用于其它对象拦截我的消息
//
class MsgHook {
public:
  MsgHook() {
    pObj = nullptr;
    nMsgMapIDToHook = 0;
    nMsgMapIDToNotify = 0;
  }

  IMessage *pObj;        // 记录谁要拦截我的消息
  int nMsgMapIDToHook;   // pObj要HOOK该map id的消息
  int nMsgMapIDToNotify; // HOOK到的消息，pObj使用该map id去响应
};

class MsgNotify {
public:
  MsgNotify() {
    pObj = nullptr;
    nMsgMapIDToNotify = 0;
  }

  IMessage *pObj;
  int nMsgMapIDToNotify; // 当有消息通知pObj时，pObj使用该id去响应
};

// 消息基类,object从该类继承从而拥有了消息功能
class Message {
public:
  Message(IMessage *);
  virtual ~Message();

  IMessage *GetIMessage();

  bool IsMsgHandled() const;
  void SetMsgHandled(bool);
  UIMSG *GetCurMsg() { return m_pCurMsg; }
  void SetCurMsg(UIMSG *p) { m_pCurMsg = p; }

  void ClearNotify();
  void SetNotify(IMessage *pObj, int nMsgMapID = 0);
  void CopyNotifyTo(IMessage *pObjCopyTo);
  IMessage *GetNotifyObj() { return m_objNotify.pObj; }

  void AddHook(IMessage *pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
  void RemoveHook(IMessage *pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
  void RemoveHook(IMessage *pObj);
  void ClearHook();

  long SendMessage(uint message, long wParam = 0, long lParam = 0);

  // 返回TRUE，表示该消息已被处理，FALSE表示该消息没被处理
  bool ProcessMessage(UIMSG *pMsg, int nMsgMapID = 0, bool bDoHook = false);
  virtual bool virtualProcessMessage(UIMSG *pMsg, int nMsgMapID = 0,
                                     bool bDoHook = false);

  bool DoHook(UIMSG *pMsg, int nMsgMapID);
  long DoNotify(UIMSG *pMsg);

  // void AddDelayRef(void** pp);
  // void RemoveDelayRef(void** pp);
  // void ResetDelayRef();

protected:
  // 例如ComboBox要hook Combobox中的下拉按钮的事件
  std::list<MsgHook *> m_lHookMsgMap; 
      
  // 产生事件时，需要通知的对象
  MsgNotify m_objNotify; 

  // 需要延迟调用自己的一些引用，避免自己被销毁之后还调用IMessage的一些函数，如uipostmessage,
  // tooltip timer. 取代原UIApplication中的AddUIObject功能（效率太低
  // std::list<void**>  m_lDelayRefs;     

  UIMSG *m_pCurMsg; // 记录当前正在处理的消息
  IMessage *m_pIMessage;
  bool m_bCreateIMessage;
};

} // namespace ui

#endif