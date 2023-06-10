#include "stdafx.h"
#include "message.h"

namespace UI
{

IMessage::IMessage(E_BOOL_CREATE_IMPL b)
{
	if (b)
		m_pImpl = new Message(this);
	else
		m_pImpl = nullptr;
}

IMessage::~IMessage()
{
	SAFE_DELETE(m_pImpl);
}

Message*  IMessage::GetImpl()
{
	return m_pImpl;
}
 
BOOL  IMessage::ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) 
{
	UI::UIMSG*  pOldMsg = nullptr;
	if (m_pImpl)
		pOldMsg = m_pImpl->GetCurMsg();

    BOOL bRet = virtualProcessMessage(pMsg, nMsgMapID, bDoHook);

	if (m_pImpl)
		m_pImpl->SetCurMsg(pOldMsg);

    return bRet;
}

void  IMessage::Release()
{
    this->virtual_delete_this();
}

BOOL    IMessage::IsMsgHandled()const
{
	return m_pImpl->IsMsgHandled();
}
void    IMessage::SetMsgHandled(BOOL b)
{
	m_pImpl->SetMsgHandled(b);
}
UIMSG*  IMessage::GetCurMsg()
{
	return m_pImpl->GetCurMsg();
}
void    IMessage::SetCurMsg(UIMSG* p)
{
	m_pImpl->SetCurMsg(p);
}
BOOL    IMessage::DoHook(UIMSG* pMsg, int nMsgMapID)
{
	return m_pImpl->DoHook(pMsg, nMsgMapID);
}


BOOL  IMessage::virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
{
    return FALSE;
}

void  IMessage::virtual_delete_this()
{
	delete this;
}

// BOOL  IMessage::DoHook(UIMSG* pMsg, int nMsgMapID) 
// { 
//     return m_pImpl->DoHook(pMsg, nMsgMapID); 
// }

void  IMessage::ClearNotify()
{ 
    return m_pImpl->ClearNotify(); 
}
void  IMessage::SetNotify(IMessage* pObj, int nMsgMapID) 
{ 
    return m_pImpl->SetNotify(pObj, nMsgMapID); 
}
void  IMessage::CopyNotifyTo(IMessage* pObjCopyTo)
{
	if (nullptr == pObjCopyTo)
		return;

	return m_pImpl->CopyNotifyTo(pObjCopyTo);
}
LONG_PTR  IMessage::DoNotify(UIMSG* pMsg) 
{
    return m_pImpl->DoNotify(pMsg); 
}

IMessage*  IMessage::GetNotifyObj()
{
    return m_pImpl->GetNotifyObj();
}

void  IMessage::AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify ) 
{ 
    if (nullptr == pObj)
        return;

    return m_pImpl->AddHook(pObj/*->GetMessageImpl()*/, nMsgMapIDToHook, nMsgMapIDToNotify); 
}
void  IMessage::RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify ) 
{ 
    if (nullptr == pObj)
        return;

    return m_pImpl->RemoveHook(pObj/*->GetMessageImpl()*/, nMsgMapIDToHook, nMsgMapIDToNotify); 
}
void  IMessage::RemoveHook(IMessage* pObj) 
{ 
    if (!pObj)
        return;

    return m_pImpl->RemoveHook(pObj/*->GetMessageImpl()*/); 
}
void  IMessage::ClearHook() 
{ 
    return m_pImpl->ClearHook(); 
}

void  IMessage::AddDelayRef(void** pp)
{
    if (!m_pImpl)
    {
        m_pImpl = new Message(this);
    }
    return m_pImpl->AddDelayRef(pp);
}
void  IMessage::RemoveDelayRef(void** pp)
{
    if (m_pImpl)
        m_pImpl->RemoveDelayRef(pp);
}

void*  IMessage::QueryInterface(REFIID iid)
{
    return (void*)UISendMessage(this, UI_MSG_QUERYINTERFACE, (WPARAM)&iid);
}

MessageProxy::MessageProxy(IMessage* p) 
	: m_pImpl(p->GetImpl())
{

}
MessageProxy::~MessageProxy() 
{

}

BOOL    MessageProxy::IsMsgHandled()const   
{ 
	return m_pImpl->IsMsgHandled();
}
void    MessageProxy::SetMsgHandled(BOOL b) 
{
	return m_pImpl->SetMsgHandled(b);
}
UIMSG*  MessageProxy::GetCurMsg()          
{
	return m_pImpl->GetCurMsg();
}
void    MessageProxy::SetCurMsg(UIMSG* p)   
{
	m_pImpl->SetCurMsg(p);
}
BOOL    MessageProxy::DoHook(UIMSG* pMsg, int nMsgMapID)
{ 
	return m_pImpl->DoHook(pMsg, nMsgMapID);
}


}