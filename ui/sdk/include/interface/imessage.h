#ifndef _UI_IMESSAGE_H_
#define _UI_IMESSAGE_H_
#include <string.h>
#include "../base/uidefine.h"
#include "include/common/guid/guid.h"

namespace ui
{
struct  IMessage;

struct MSG {
#if defined(OS_WIN)
    HWND hWnd;
#else
    long hWnd;
#endif
    uint message;
    long wParam;
    long lParam;
};

// ��Ϣ�ṹ���塣ϵͳ�ؼ���ȻʹuiSG����UI�ؼ�ʹ��UIMsg���д���
struct UIMSG : public MSG
{
    UIMSG() { memset(this, 0, sizeof(UIMSG)); }

    IMessage*   pMsgFrom;    // ��Ϣ������
    IMessage*   pMsgTo;      // ��Ϣ������ 
    uint        nCode;       // ��� WM_COMMAND,WM_NOTIFY
	long        lRet;        // ��Ϣ���������ķ���ֵ
    bool        bHandled;    // ����Ϣ�Ƿ��ѱ������
};



class Message;
struct UIAPI IMessage
{
    IMessage(E_BOOL_CREATE_IMPL);
    bool  ProcessMessage(UIMSG* pMsg, int nMsgMapID=0, bool bDoHook=false);
    void  Release();
	
	bool    IsMsgHandled()const;
	void    SetMsgHandled(bool b);
	UIMSG*  GetCurMsg();
	void    SetCurMsg(UIMSG* p);
	bool    DoHook(UIMSG* pMsg, int nMsgMapID);

protected:
    friend class Message;  
    virtual ~IMessage();  // �麯��1. ��֤��ȷ�ͷ���������
private:
    virtual bool  virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook);  // �麯��2. ��Ϣ����
	virtual void  virtual_delete_this();  // ��UIObjCreator����ʵ��

public:
	Message*  GetImpl();
    void  ClearNotify();
    void  SetNotify(IMessage* pObj, int nMsgMapID);
	long  DoNotify(UIMSG* pMsg);
    IMessage*  GetNotifyObj();
	void  CopyNotifyTo(IMessage* pObjCopyTo);

    void  AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void  RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void  RemoveHook(IMessage* pObj );
    void  ClearHook();

    void  AddDelayRef(void** pp);
    void  RemoveDelayRef(void** pp);

    void*  QueryInterface(const Guid& iid);
protected:
    Message*   m_pImpl;
};


// ���ⲿ����̳У�����֧����Ϣӳ��궨��
class UIAPI MessageProxy
{
public:
	MessageProxy(IMessage* p);
	virtual ~MessageProxy();

	bool    IsMsgHandled()const;
	void    SetMsgHandled(bool b);
	UIMSG*  GetCurMsg();
	void    SetCurMsg(UIMSG* p);
	bool    DoHook(UIMSG* pMsg, int nMsgMapID);

protected:
	Message*  m_pImpl;
};

}

#endif  // _UI_IMESSAGE_H_