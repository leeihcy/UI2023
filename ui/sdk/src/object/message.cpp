#include "message.h"
#include "util/log.h"

namespace ui
{
//
//	return
//		����pMsg->lRet��Ϊ����ֵ��
//			�¼���Ӧ�����Ľ������ֵ��ΪpMsg->lRet;
//			����¼���Ӧ����û�з���ֵ��pMsg->lRetĬ��Ϊ0
//	remark
//		��Ҫ֪�������Ϣ��û�б������ɵ���IsMsgHandled()
//
//  ע������ϵͳ��SendMessage������nMsg�з�Χ���Ƶģ�
//     if(Msg&0xFFFE0000)// Msgs (>0xFFFF) are reserved by the system   
//     {  
//         User32._UserSetLaseError(0x57); //0x57 The parameter is incorrect.    
//         return 0;  
//     }  
long UISendMessage(UIMSG* pMsg, int nMsgMapID, bool* pbHandled)
{
	UIASSERT (pMsg != nullptr);
	if (nullptr == pMsg)
	{
		UI_LOG_ERROR(L"UISendMessage, pMsg==nullptr! ");
		return -1;
	}
	
	UIASSERT(pMsg->pMsgTo != nullptr);
//	UIASSERT(pMsg->message != 0);   // �п�����WM_NULL

	if (nullptr == pMsg->pMsgTo)
	{
		UI_LOG_ERROR(L"UISendMessage, pMessageTo==nullptr!  message=%d", pMsg->message);
		return -1;
	}

	bool bRet = pMsg->pMsgTo->ProcessMessage(pMsg, nMsgMapID, true);
	if (pbHandled) 
	{
		*pbHandled = bRet;
	}

	return pMsg->lRet;
}

long UISendMessage(IMessage* pMsgTo,  uint message,
					long wParam, long lParam, 
					uint nCode, IMessage* pMsgFrom,
					int nMsgMapID,bool* pbHandled)
{
	if (nullptr == pMsgTo)
	{
		UI_LOG_FATAL(L"pObjMsgTo == nullptr");
		return 0;
	}

    UIMSG msg;
	msg.pMsgFrom = pMsgFrom;
	msg.pMsgTo   = pMsgTo;

	msg.message = message;
	msg.nCode   = nCode;
	msg.wParam  = wParam;
	msg.lParam  = lParam;

	bool bRet = pMsgTo->ProcessMessage(&msg, nMsgMapID, true);
	if (pbHandled) 
	{
		*pbHandled = bRet;
	}

	return msg.lRet;
}

long UISendMessage(Message* pMsgTo, uint message, long wParam, long lParam)
{
	if (nullptr == pMsgTo)
		return 0;

	return UISendMessage(pMsgTo->GetIMessage(), message, wParam, lParam, 0, nullptr, 0, nullptr);
}


// ��֧�ֿ��̵߳���
long UIPostMessage(HWND hForwardMsgWnd, UIMSG* pMsg, int nMsgMapID)
{
	if (!pMsg->pMsgTo)
		return 0;

	UIMSG* pCloneMsg = new UIMSG;
	memcpy(pCloneMsg, pMsg, sizeof(UIMSG));

	pMsg->pMsgTo->AddDelayRef((void**)&pCloneMsg->pMsgTo);

	// -->ForwardPostMessageWindow::ProcessWindowMessage
#if defined(OS_WIN)
	::PostMessage(hForwardMsgWnd, UI_MSG_POSTMESSAGE, (long)pCloneMsg, (long)nMsgMapID);
#else 
    UIASSERT(0);
#endif

	return 0;
}

// ����pMsg.pObjTo��һ��Message���ͣ��޷���ȡpUIAppָ�룬ֻ��������һ������
long  UIPostMessage(IUIApplication* pUIApp, UIMSG* pMsg, int nMsgMapID)
{
    if (nullptr == pUIApp)
        return 0;
#if defined(OS_WIN)
    HWND hDestWnd = pUIApp->GetForwardPostMessageWnd();
    if (nullptr == hDestWnd)
        return 0;

    return UIPostMessage(hDestWnd, pMsg, nMsgMapID);
#else
    // NOT IMPL
    UIASSERT(false);
    return 0;
#endif
}


Message::Message(IMessage* pIMessage)
{
	m_pCurMsg = nullptr;
    m_pIMessage = pIMessage;
    m_bCreateIMessage = false;
}
Message::~Message()
{
	m_pCurMsg = nullptr;

	this->ClearNotify();
	this->ClearHook();
    this->ResetDelayRef();

    if (m_bCreateIMessage)
    {
		m_pIMessage->m_pImpl = nullptr;
        SAFE_DELETE(m_pIMessage);
    }
}

// ���Լ�û�д�IMessage����ʱ��Ϊ��֧��IMessage���Լ�����һ����
IMessage*  Message::GetIMessage()   
{ 
//     if (nullptr == m_pIMessage)
//     {
//         m_pIMessage = new IMessageInnerProxy;
//         m_pIMessage->SetMessageImpl(this);
//         m_bCreateIMessage = true;
//     }
    return m_pIMessage; 
}


// ��ȡ��ǰ��Ϣ�Ƿ��ѱ��������
// remark
//	  �ú���ֻ����ProcessMessage��Χ�ڵ��ã���Ϊm_pCurMsgֻ����η�Χ����Ч
bool Message::IsMsgHandled() const                         
{                  
	UIASSERT(m_pCurMsg);
    if (!m_pCurMsg)
        return false;

	return this->m_pCurMsg->bHandled;                         
}   

// ���õ�ǰ��Ϣ�ѱ��������
void Message::SetMsgHandled(bool bHandled)
{
    UIASSERT(m_pCurMsg);	
    if (!m_pCurMsg)
        return;

    this->m_pCurMsg->bHandled = bHandled; 
}

void Message::SetNotify(IMessage* pObj, int nMsgMapID)
{
	m_objNotify.pObj = pObj;
	m_objNotify.nMsgMapIDToNotify = nMsgMapID;
}

void Message::ClearNotify()
{
	m_objNotify.pObj = nullptr;
	m_objNotify.nMsgMapIDToNotify = 0;
}

//
//  e.g. �˵������ڵ���ʱ����Ҫ����һ�θ����ڵ�notify����
//
void Message::CopyNotifyTo(IMessage* pObjCopyTo)
{
	if (nullptr == pObjCopyTo)
		return;

	pObjCopyTo->SetNotify(m_objNotify.pObj, m_objNotify.nMsgMapIDToNotify);
}

// ����Notify��Ϣ��ע���Notify�Ķ���.(�޸�Ϊֻ֪ͨ��һ����.֪ͨ�����̫����)
// ��bPostΪtrueʱ������ҪpUIApp����
long Message::DoNotify(UIMSG* pMsg/*, bool bPost, IUIApplication* pUIApp*/)
{
//	IMessage* pNotifyObj = pMsg->pMsgTo;
	int    nMsgMapID = 0;
	long   lRet = 0;     // �����UIMSG��ָ����pObjMsgTo�������ȷ��͸��ö��󣬲���nMsgMapID = 0;

	if (nullptr == pMsg->pMsgTo)
	{
		pMsg->pMsgTo = m_objNotify.pObj/*->GetIMessage()*/;
		nMsgMapID = m_objNotify.nMsgMapIDToNotify;
	}
	if (nullptr == pMsg->pMsgTo)
	{
		return 0;
	}

// 	if (bPost && nullptr != pUIApp)
// 	{
// 		UIPostMessage(pUIApp, pMsg, nMsgMapID);
// 		lRet = 0;
// 	}
// 	else
	{
		lRet = UISendMessage(pMsg, nMsgMapID, nullptr);
	}
	return lRet;
}



// 
//	����Ϣ����HOOK����
//
//	return
//		�����е�HOOK�б��У�ֻҪ��һ��HOOK��������󷵻�TRUE��������
//		HOOK���̽�����
//	remark
//		��HOOK�����У����� pMsg->pObjMsgTo���޸�
//
bool Message::DoHook( UIMSG* pMsg, int nMsgMapID )
{
	bool  bRet = false;

	if (0 == m_lHookMsgMap.size())
		return bRet;

	std::list<MsgHook*>::iterator  iter = m_lHookMsgMap.begin();
	for (; iter!=m_lHookMsgMap.end(); )
	{
		MsgHook* pHook = *iter;
		iter++;  // ��++���������Ϊ�˽����ProccessMessage�е���RemoveHook->erase(iter)�ı�������

		if (nMsgMapID == pHook->nMsgMapIDToHook)
		{
			if (true == pHook->pObj->ProcessMessage(pMsg, (int)pHook->nMsgMapIDToNotify))
				return true;
		}
	}

	return false;
}

void Message::AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify )
{
	std::list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	std::list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgHook* pHook = *iter;

		if( pObj == pHook->pObj &&
			nMsgMapIDToHook == pHook->nMsgMapIDToHook &&
			nMsgMapIDToNotify == pHook->nMsgMapIDToNotify )
			return;
	}

	MsgHook*  pHook = new MsgHook;
	pHook->pObj = pObj;
	pHook->nMsgMapIDToHook = nMsgMapIDToHook;
	pHook->nMsgMapIDToNotify = nMsgMapIDToNotify;

	this->m_lHookMsgMap.push_back( pHook );
}

void Message::RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify )
{
	std::list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	std::list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for (; iter!=iterEnd; iter++)
	{
		MsgHook* pHook = *iter;

		if (pObj == pHook->pObj &&
			nMsgMapIDToHook == pHook->nMsgMapIDToHook &&
			nMsgMapIDToNotify == pHook->nMsgMapIDToNotify)
		{
			
			delete    pHook;
			this->m_lHookMsgMap.erase( iter );

			return;
		}
	}
}
void Message::RemoveHook(IMessage* pObj)
{
	std::list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	std::list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for (; iter!=iterEnd;)
	{
		MsgHook*  pHook = *iter;
		if (pObj == pHook->pObj)
		{
			delete    pHook;
			iter = this->m_lHookMsgMap.erase( iter );
			continue;
		}
		iter++;
	}
}

void Message::ClearHook( )
{
	std::list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	std::list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for (; iter!=iterEnd; iter++)
	{
		MsgHook*  pHook = *iter;
		delete    pHook;
	}
	this->m_lHookMsgMap.clear();
}

void  Message::AddDelayRef(void** pp)
{
    std::list<void**>::iterator  iter = std::find(m_lDelayRefs.begin(), m_lDelayRefs.end(), pp);
    if (iter == m_lDelayRefs.end())
    {
        m_lDelayRefs.push_back(pp);
    }
}
void  Message::RemoveDelayRef(void** pp)
{
    std::list<void**>::iterator  iter = std::find(m_lDelayRefs.begin(), m_lDelayRefs.end(), pp);
    if (iter != m_lDelayRefs.end())
    {
        m_lDelayRefs.erase(iter);
    }
}
void  Message::ResetDelayRef()
{
    std::list<void**>::iterator  iter = m_lDelayRefs.begin();
    for (; iter != m_lDelayRefs.end(); iter++)
    {
        void** pp = *iter;
        *pp = nullptr;
    }
    m_lDelayRefs.clear();
}

 bool Message::ProcessMessage(
        UIMSG* pMsg, int nMsgMapID, bool bDoHook)
 {
     ui::UIMSG*  pOldMsg  = this->GetCurMsg(); 
     bool bRet = this->virtualProcessMessage(pMsg, nMsgMapID, bDoHook);
     this->SetCurMsg(pOldMsg);    
     return bRet;
 }
 bool Message::virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
 {
     return false; 
 }

}