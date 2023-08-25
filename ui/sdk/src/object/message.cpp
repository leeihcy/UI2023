#include <string>

#include "include/macro/helper.h"
#include "include/util/log.h"

#include "message.h"


namespace ui {

Message::Message(IMessage *pIMessage) {
  m_pCurMsg = nullptr;
  m_pIMessage = pIMessage;
  m_bCreateIMessage = false;
}
Message::~Message() {
  m_pCurMsg = nullptr;

  this->ClearNotify();
  this->ClearHook();
  // this->ResetDelayRef();

  if (m_bCreateIMessage) {
    m_pIMessage->m_pImpl = nullptr;
    SAFE_DELETE(m_pIMessage);
  }
}


void Message::RouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    m_meta = static_cast<FinalConstructMessage*>(msg)->meta;
    return;
  }
}

// 当自己没有从IMessage创建时，为了支持IMessage，自己创建一个。
IMessage *Message::GetIMessage() {
  //     if (nullptr == m_pIMessage)
  //     {
  //         m_pIMessage = new IMessageInnerProxy;
  //         m_pIMessage->SetMessageImpl(this);
  //         m_bCreateIMessage = true;
  //     }
  return m_pIMessage;
}

IMeta *Message::GetMeta() { return m_meta; }

void Message::connect(const char *event_name, slot<void(Event *)> &&s) {
  if (!event_name || !event_name[0]) {
    return;
  }
  m_events[std::string(event_name)].connect(
      std::forward<slot<void(Event *)>>(s));
}

void Message::emit(const char *event_name, Event *event) {
  if (!event_name || !event_name[0]) {
    return;
  }
  auto iter = m_events.find(std::string(event_name));
  if (iter == m_events.end()) {
    return;
  }
  iter->second.emit(event);
}

llong Message::SendMessage(uint message, llong wParam, llong lParam)
{
  return m_pIMessage->SendMessage(message, wParam, lParam);
}

// 获取当前消息是否已被处理过了
// remark
//	  该函数只能在ProcessMessage范围内调用，因为m_pCurMsg只在这段范围内有效
bool Message::IsMsgHandled() const {
  UIASSERT(m_pCurMsg);
  if (!m_pCurMsg)
    return false;

  return this->m_pCurMsg->bHandled;
}

// 设置当前消息已被被处理过
void Message::SetMsgHandled(bool bHandled) {
  UIASSERT(m_pCurMsg);
  if (!m_pCurMsg)
    return;

  this->m_pCurMsg->bHandled = bHandled;
}

void Message::SetNotify(IMessage *pObj, int nMsgMapID) {
  m_objNotify.pObj = pObj;
  m_objNotify.nMsgMapIDToNotify = nMsgMapID;
}

void Message::ClearNotify() {
  m_objNotify.pObj = nullptr;
  m_objNotify.nMsgMapIDToNotify = 0;
}

//
//  e.g. 菜单子项在弹出时，需要拷贝一次父窗口的notify对象
//
void Message::CopyNotifyTo(IMessage *pObjCopyTo) {
  if (nullptr == pObjCopyTo)
    return;

  pObjCopyTo->SetNotify(m_objNotify.pObj, m_objNotify.nMsgMapIDToNotify);
}

// 发送Notify消息到注册过Notify的对象.(修改为只通知给一个人.通知多个人太乱了)
// 当bPost为true时，才需要pUIApp参数
llong Message::DoNotify(UIMSG *pMsg /*, bool bPost, IApplication* pUIApp*/) {
  //	IMessage* pNotifyObj = pMsg->pMsgTo;
  int nMsgMapID = 0;
  llong lRet =
      0; // 如果在UIMSG中指定了pObjMsgTo，则优先发送给该对象，并且nMsgMapID = 0;

  if (nullptr == pMsg->pMsgTo) {
    pMsg->pMsgTo = m_objNotify.pObj /*->GetIMessage()*/;
    nMsgMapID = m_objNotify.nMsgMapIDToNotify;
  }
  if (nullptr == pMsg->pMsgTo) {
    return 0;
  }

  // 	if (bPost && nullptr != pUIApp)
  // 	{
  // 		UIPostMessage(pUIApp, pMsg, nMsgMapID);
  // 		lRet = 0;
  // 	}
  // 	else
  { lRet = pMsg->pMsgTo->SendMessage(pMsg, nMsgMapID, nullptr); }
  return lRet;
}

//
//	对消息进行HOOK处理
//
//	return
//		在所有的HOOK列表中，只要有一个HOOK处理结束后返回TRUE，则整个
//		HOOK过程结束。
//	remark
//		在HOOK过程中，不对 pMsg->pObjMsgTo作修改
//
bool Message::DoHook(UIMSG *pMsg, int nMsgMapID) {
  bool bRet = false;

  if (0 == m_lHookMsgMap.size())
    return bRet;

  std::list<MsgHook *>::iterator iter = m_lHookMsgMap.begin();
  for (; iter != m_lHookMsgMap.end();) {
    MsgHook *pHook = *iter;
    iter++; // 将++放在这里，是为了解决在ProccessMessage中调用RemoveHook->erase(iter)的崩溃问题

    if (nMsgMapID == pHook->nMsgMapIDToHook) {
      if (true ==
          pHook->pObj->ProcessMessage(pMsg, (int)pHook->nMsgMapIDToNotify))
        return true;
    }
  }

  return false;
}

void Message::AddHook(IMessage *pObj, int nMsgMapIDToHook,
                      int nMsgMapIDToNotify) {
  std::list<MsgHook *>::iterator iter = m_lHookMsgMap.begin();
  std::list<MsgHook *>::iterator iterEnd = m_lHookMsgMap.end();
  for (; iter != iterEnd; iter++) {
    MsgHook *pHook = *iter;

    if (pObj == pHook->pObj && nMsgMapIDToHook == pHook->nMsgMapIDToHook &&
        nMsgMapIDToNotify == pHook->nMsgMapIDToNotify)
      return;
  }

  MsgHook *pHook = new MsgHook;
  pHook->pObj = pObj;
  pHook->nMsgMapIDToHook = nMsgMapIDToHook;
  pHook->nMsgMapIDToNotify = nMsgMapIDToNotify;

  this->m_lHookMsgMap.push_back(pHook);
}

void Message::RemoveHook(IMessage *pObj, int nMsgMapIDToHook,
                         int nMsgMapIDToNotify) {
  std::list<MsgHook *>::iterator iter = m_lHookMsgMap.begin();
  std::list<MsgHook *>::iterator iterEnd = m_lHookMsgMap.end();
  for (; iter != iterEnd; iter++) {
    MsgHook *pHook = *iter;

    if (pObj == pHook->pObj && nMsgMapIDToHook == pHook->nMsgMapIDToHook &&
        nMsgMapIDToNotify == pHook->nMsgMapIDToNotify) {

      delete pHook;
      this->m_lHookMsgMap.erase(iter);

      return;
    }
  }
}
void Message::RemoveHook(IMessage *pObj) {
  std::list<MsgHook *>::iterator iter = m_lHookMsgMap.begin();
  std::list<MsgHook *>::iterator iterEnd = m_lHookMsgMap.end();
  for (; iter != iterEnd;) {
    MsgHook *pHook = *iter;
    if (pObj == pHook->pObj) {
      delete pHook;
      iter = this->m_lHookMsgMap.erase(iter);
      continue;
    }
    iter++;
  }
}

void Message::ClearHook() {
  std::list<MsgHook *>::iterator iter = m_lHookMsgMap.begin();
  std::list<MsgHook *>::iterator iterEnd = m_lHookMsgMap.end();
  for (; iter != iterEnd; iter++) {
    MsgHook *pHook = *iter;
    delete pHook;
  }
  this->m_lHookMsgMap.clear();
}

// void Message::AddDelayRef(void **pp) {
//   std::list<void **>::iterator iter =
//       std::find(m_lDelayRefs.begin(), m_lDelayRefs.end(), pp);
//   if (iter == m_lDelayRefs.end()) {
//     m_lDelayRefs.push_back(pp);
//   }
// }
// void Message::RemoveDelayRef(void **pp) {
//   std::list<void **>::iterator iter =
//       std::find(m_lDelayRefs.begin(), m_lDelayRefs.end(), pp);
//   if (iter != m_lDelayRefs.end()) {
//     m_lDelayRefs.erase(iter);
//   }
// }
// void Message::ResetDelayRef() {
//   std::list<void **>::iterator iter = m_lDelayRefs.begin();
//   for (; iter != m_lDelayRefs.end(); iter++) {
//     void **pp = *iter;
//     *pp = nullptr;
//   }
//   m_lDelayRefs.clear();
// }

bool Message::ProcessMessage(UIMSG *pMsg, int nMsgMapID, bool bDoHook) {
  ui::UIMSG *pOldMsg = this->GetCurMsg();
  bool bRet = this->virtualProcessMessage(pMsg, nMsgMapID, bDoHook);
  this->SetCurMsg(pOldMsg);
  return bRet;
}
bool Message::virtualProcessMessage(UIMSG *pMsg, int nMsgMapID, bool bDoHook) {
  return false;
}

} // namespace ui