#ifndef _UI_SDK_SRC_OBJECT_MESSAGE_H_
#define _UI_SDK_SRC_OBJECT_MESSAGE_H_

#include "include/interface/imessage.h"
#include <list>

namespace ui
{
	//
	// �����������������ҵ���Ϣ
	//
	class MsgHook
	{
	public:
		MsgHook()
		{
			pObj = nullptr;
			nMsgMapIDToHook = 0;
			nMsgMapIDToNotify = 0;
		}

		IMessage* pObj;                // ��¼˭Ҫ�����ҵ���Ϣ
		int      nMsgMapIDToHook;      // pObjҪHOOK��map id����Ϣ
		int      nMsgMapIDToNotify;    // HOOK������Ϣ��pObjʹ�ø�map idȥ��Ӧ
	};

	class MsgNotify
	{
	public:
		MsgNotify()
		{
			pObj = nullptr;
			nMsgMapIDToNotify = 0;
		}

		IMessage*  pObj;
		int       nMsgMapIDToNotify;    // ������Ϣ֪ͨpObjʱ��pObjʹ�ø�idȥ��Ӧ
	};

	// ��Ϣ����,object�Ӹ���̳дӶ�ӵ������Ϣ����
	class Message
	{
	public:
		Message(IMessage*);
		virtual ~Message();

		IMessage*    GetIMessage();

		bool         IsMsgHandled()const;
		void         SetMsgHandled(bool);
		UIMSG*       GetCurMsg() { return m_pCurMsg; }
		void         SetCurMsg(UIMSG* p) { m_pCurMsg = p; }

		void         ClearNotify();
		void         SetNotify(IMessage* pObj, int nMsgMapID = 0);
		void         CopyNotifyTo(IMessage* pObjCopyTo);
		IMessage*    GetNotifyObj() { return m_objNotify.pObj; }

		void         AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
		void         RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify);
		void         RemoveHook(IMessage* pObj);
		void         ClearHook();

    long SendMessage(uint message, long wParam = 0, long lParam = 0);

		// ����TRUE����ʾ����Ϣ�ѱ�����FALSE��ʾ����Ϣû������
		bool         ProcessMessage(UIMSG* pMsg, int nMsgMapID = 0, bool bDoHook = false);
		virtual bool virtualProcessMessage(UIMSG* pMsg, int nMsgMapID = 0, bool bDoHook = false);

		bool         DoHook(UIMSG* pMsg, int nMsgMapID);
		long         DoNotify(UIMSG* pMsg);

		void         AddDelayRef(void** pp);
		void         RemoveDelayRef(void** pp);
		void         ResetDelayRef();

	protected:
		std::list<MsgHook*>  m_lHookMsgMap;      // ����ComboBoxҪhook Combobox�е�������ť���¼�
		MsgNotify          m_objNotify;        // �����¼�ʱ����Ҫ֪ͨ�Ķ���
		std::list<void**>  m_lDelayRefs;       // ��Ҫ�ӳٵ����Լ���һЩ���ã������Լ�������֮�󻹵���IMessage��һЩ��������uipostmessage, tooltip timer. ȡ��ԭUIApplication�е�AddUIObject���ܣ�Ч��̫��

		UIMSG *           m_pCurMsg;          // ��¼��ǰ���ڴ������Ϣ
		IMessage*         m_pIMessage;
		bool              m_bCreateIMessage;

    
	};

}

#endif