#pragma once
namespace UI
{
	//
	// 用于其它对象拦截我的消息
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

		IMessage* pObj;                // 记录谁要拦截我的消息
		int      nMsgMapIDToHook;      // pObj要HOOK该map id的消息
		int      nMsgMapIDToNotify;    // HOOK到的消息，pObj使用该map id去响应
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
		int       nMsgMapIDToNotify;    // 当有消息通知pObj时，pObj使用该id去响应
	};

	// 消息基类,object从该类继承从而拥有了消息功能
	class Message
	{
	public:
		Message(IMessage*);
		virtual ~Message();

		IMessage*    GetIMessage();

		BOOL         IsMsgHandled()const;
		void         SetMsgHandled(BOOL);
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

		// 返回TRUE，表示该消息已被处理，FALSE表示该消息没被处理
		BOOL         ProcessMessage(UIMSG* pMsg, int nMsgMapID = 0, bool bDoHook = false);
		virtual BOOL virtualProcessMessage(UIMSG* pMsg, int nMsgMapID = 0, bool bDoHook = false);

		BOOL         DoHook(UIMSG* pMsg, int nMsgMapID);
		LONG_PTR     DoNotify(UIMSG* pMsg);

		void         AddDelayRef(void** pp);
		void         RemoveDelayRef(void** pp);
		void         ResetDelayRef();

	protected:
		list<MsgHook*>    m_lHookMsgMap;      // 例如ComboBox要hook Combobox中的下拉按钮的事件
		MsgNotify         m_objNotify;        // 产生事件时，需要通知的对象
		list<void**>      m_lDelayRefs;       // 需要延迟调用自己的一些引用，避免自己被销毁之后还调用IMessage的一些函数，如uipostmessage, tooltip timer. 取代原UIApplication中的AddUIObject功能（效率太低

		UIMSG *           m_pCurMsg;          // 记录当前正在处理的消息
		IMessage*         m_pIMessage;
		BOOL              m_bCreateIMessage;
	};

}