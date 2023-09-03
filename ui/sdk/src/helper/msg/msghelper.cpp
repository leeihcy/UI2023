#include "msghelper.h"
#include "src/object/message.h"
#include "src/application/uiapplication.h"

namespace ui {
#if 0 // defined(OS_WIN)
BOOL ForwardPostMessageWindow::ProcessWindowMessage(HWND hWnd, unsigned int uMsg, long wParam, long lParam, long& lResult, unsigned int dwMsgMapID)
{
	if (UI_MSG_POSTMESSAGE == uMsg)
	{
		UIMSG* pMsg = (UIMSG*)wParam;
		if (!pMsg->pMsgTo)  // �п��ܸö����ѱ�ɾ��
		{                                                 
			delete pMsg;
			return TRUE;
		}

		int msgMapId = (int)lParam;
		UISendMessage(pMsg, msgMapId);
        pMsg->pMsgTo->RemoveDelayRef((void**)&(pMsg->pMsgTo));
		delete pMsg;

		return TRUE;
	}
	else if (WM_DESTROY == uMsg)  // ��ʣ��δ�������post��Ϣ�ͷţ������ڴ�й¶
	{
        // Note that PeekMessage always retrieves WM_QUIT messages, 
        // no matter which values you specify for wMsgFilterMin and
        // wMsgFilterMax.

		MSG  msg;
		while (::PeekMessage(&msg, m_hWnd, UI_MSG_POSTMESSAGE, UI_MSG_POSTMESSAGE, PM_REMOVE))
		{
            if (msg.message == UI_MSG_POSTMESSAGE)
            {
                UIMSG* pMsg = (UIMSG*)msg.wParam;
                if (pMsg->pMsgTo)
                    pMsg->pMsgTo->RemoveDelayRef((void**)&(pMsg->pMsgTo));
                delete pMsg;
            }
            else
            {
                break;
            }
		}
	}

	return FALSE;
}

WaitForHandle::WaitForHandle(HANDLE h, IWaitForHandleCallback* pCB, long l)
{
	m_hHandle = h;
	m_pCallback = pCB;
	m_lParam = l;
}

WaitForHandlesMgr::WaitForHandlesMgr()
{
	m_pHandles = nullptr;
	m_nHandleCount = 0;
}
WaitForHandlesMgr::~WaitForHandlesMgr()
{
	_MyIter	iter = m_list.begin();
	_MyIter	iterEnd = m_list.end();
	for (; iter != iterEnd; iter++)
	{
		delete (*iter);
	}
	m_list.clear();
	m_nHandleCount = 0;

	SAFE_ARRAY_DELETE(m_pHandles);
	m_nHandleCount = 0;
}

WaitForHandle* WaitForHandlesMgr::FindHandle(HANDLE h)
{
	if (nullptr == h)
		return nullptr;

	_MyIter	iter = m_list.begin();
	_MyIter	iterEnd = m_list.end();

	for (;iter != iterEnd; iter++)
		if (h == (*iter)->GetHandle())
			return *iter;

	return nullptr;
}
list<WaitForHandle*>::iterator WaitForHandlesMgr::FindHandleIter(HANDLE h)
{
	if (nullptr == h)
		return m_list.end();

	_MyIter	iter = m_list.begin();
	_MyIter	iterEnd = m_list.end();

	for (;iter != iterEnd; iter++)
		if (h == (*iter)->GetHandle())
			return iter;

	return m_list.end();
}
bool WaitForHandlesMgr::AddHandle(HANDLE h, IWaitForHandleCallback* pCB, long l)
{
	if (nullptr == h || nullptr == pCB)
		return false;

	if (FindHandle(h))
		return false;

	WaitForHandle* p = new WaitForHandle(h, pCB, l);
	m_list.push_back(p);

	UpdateHandleArray();
	return true;
}
bool WaitForHandlesMgr::RemoveHandle(HANDLE h)
{
	if (nullptr == h)
		return false;

	_MyIter iter = FindHandleIter(h);
	if (iter == m_list.end())
		return false;

	WaitForHandle* p = *iter;
	SAFE_DELETE(p);
	m_list.erase(iter);

	UpdateHandleArray();
	return true;
}

bool WaitForHandlesMgr::UpdateHandleArray()
{
	SAFE_ARRAY_DELETE(m_pHandles);
	m_nHandleCount = (int)m_list.size();

	if (0 == m_nHandleCount)
		return true;

	m_pHandles = new HANDLE[m_nHandleCount];
	_MyIter	iter = m_list.begin();
	_MyIter	iterEnd = m_list.end();

	for (int i = 0; iter != iterEnd; iter++, i++)
	{
		m_pHandles [i] = (*iter)->GetHandle();
	}
	return true;
}

void WaitForHandlesMgr::Do(HANDLE h)
{
	WaitForHandle* pWaitForHandle = this->FindHandle(h);
	if (nullptr == pWaitForHandle)
		return;

	pWaitForHandle->GetCB()->OnWaitForHandleObjectCallback(h, pWaitForHandle->GetLParam());
}


// ��ʹ��stl::list���������������Լ���PreTranslateMessage��Ӧ�е���RemoveMessageFilter����
BOOL MessageFilterMgr::PreTranslateMessage(MSG* pMsg)
{
    UIListItem<IPreTranslateMessage*>* p = m_list.GetFirst();
    UIListItem<IPreTranslateMessage*>* pNext = nullptr;

    while (p)
    {
        pNext = p->GetNext();    
       
        if (p->m_data->PreTranslateMessage(pMsg))
            return TRUE;

        p = pNext;
    }

	return FALSE;
}

void  MessageFilterMgr::AddMessageFilter(IPreTranslateMessage* p)
{
    if (nullptr == p)
        return;

    m_list.Add(p);
}
void  MessageFilterMgr::RemoveMessageFilter(IPreTranslateMessage* p)
{
    if (nullptr == p)
        return;

    m_list.Remove(p);
}
#endif
}