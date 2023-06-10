#ifndef _UI_IMESSAGE_H_
#define _UI_IMESSAGE_H_

namespace UI
{
interface  IMessage;

// 消息结构定义。系统控件仍然使用MSG，但UI控件使用UIMsg进行代码
struct UIMSG : public MSG
{
    UIMSG() { memset(this, 0, sizeof(UIMSG)); }

    IMessage*   pMsgFrom;    // 消息发送者
    IMessage*   pMsgTo;      // 消息接受者 
    UINT        nCode;       // 针对 WM_COMMAND,WM_NOTIFY
	LRESULT     lRet;        // 消息处理结束后的返回值
    BOOL        bHandled;    // 该消息是否已被处理过
};



class Message;
interface UIAPI IMessage
{
    IMessage(E_BOOL_CREATE_IMPL);
    BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID=0, bool bDoHook=false);
    void  Release();
	
	BOOL    IsMsgHandled()const;
	void    SetMsgHandled(BOOL b);
	UIMSG*  GetCurMsg();
	void    SetCurMsg(UIMSG* p);
	BOOL    DoHook(UIMSG* pMsg, int nMsgMapID);

protected:
    friend class Message;  
    virtual ~IMessage();  // 虚函数1. 保证正确释放整个对象
private:
    virtual BOOL  virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook);  // 虚函数2. 消息处理
	virtual void  virtual_delete_this();  // 由UIObjCreator负责实现

public:
	Message*  GetImpl();
    void  ClearNotify();
    void  SetNotify(IMessage* pObj, int nMsgMapID);
	LONG_PTR  DoNotify(UIMSG* pMsg);
    IMessage*  GetNotifyObj();
	void  CopyNotifyTo(IMessage* pObjCopyTo);

    void  AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void  RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void  RemoveHook(IMessage* pObj );
    void  ClearHook();

    void  AddDelayRef(void** pp);
    void  RemoveDelayRef(void** pp);

    void*  QueryInterface(REFIID iid);
protected:
    Message*   m_pImpl;
};


// 由外部的类继承，用于支持消息映射宏定义
class UIAPI MessageProxy
{
public:
	MessageProxy(IMessage* p);
	virtual ~MessageProxy();

	BOOL    IsMsgHandled()const;
	void    SetMsgHandled(BOOL b);
	UIMSG*  GetCurMsg();
	void    SetCurMsg(UIMSG* p);
	BOOL    DoHook(UIMSG* pMsg, int nMsgMapID);

protected:
	Message*  m_pImpl;
};

}

#endif  _UI_IMESSAGE_H_