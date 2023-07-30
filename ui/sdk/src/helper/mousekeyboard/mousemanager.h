#pragma once

namespace ui
{
class KeyboardMgrBase;
class WindowKeyboardMgr;
class Object;
class WindowBase;
class Application;

struct GetObjectByPosExData
{
	Object* pObjParent;
	POINT*  ptParent;
	POINT*  ptOutInObj __out;  // out

	bool  bTestDecendant;
	bool  bTestRejectMouseMsg;
	bool  bTestDisable;
};

class WindowMouseMgr : public IMKMgr   // ����Ŀǰû�н�mouse/keyboard�ϲ���������ʱ��window��ʵ������ӿ�
{
public:
    WindowMouseMgr(WindowBase& o);
    ~WindowMouseMgr(void);

    Object*  GetObjectByPos(Object* pObjParent, POINT* ptWindow, __out POINT* ptObj);
	Object*  GetObjectByPos_UIEditor(Object* pObjParent, POINT* ptWindow, __out POINT* ptObj);
    static Object*  GetObjectByPosEx(GetObjectByPosExData* pData);

public:
    Application*  GetUIApplication();
    WindowBase*  GetWindowObject();
    void  SetUIApplication(Application* p);

    long  HandleMessage(unsigned int msg, long w, long l, BOOL* pbHandled);
    long  HandleMouseMessage(unsigned int msg, long w, long l, BOOL* pbHandled);
    long  HandleKeyboardMessage(unsigned int msg, long w, long l, BOOL* pbHandled);
    long  HandleTouchMessage(unsigned int msg, long w, long l, BOOL& bHandled);
    BOOL     IsDialogMessage(MSG* pMsg);

	void  ClearStateDirect();

    void  CheckDefPushButton(Object* pNewObj);
    void  Tab_2_NextControl();
    void  Tab_2_PrevControl();
    void  SetKeyboardCapture(IMessage* pObj, int nNotifyMsgId);
    void  ReleaseKeyboardCapture(IMessage* pObj);
    void  SetMouseCapture(IMessage* pObj, int nNotifyMsgId);
    void  ReleaseMouseCapture(IMessage* pObj);

	Object*  GetHoverObject();
	Object*  GetPressObject();
    Object*  GetFocusObject();
    Object*  GetDefaultObject();
    Object*  GetOldFocusObject(bool bRemove = true);
    Object*  GetOriginDefaultObject();

    void  SetFocusObject(Object* pObj);
    void  SetDefaultObject(Object* pObj, bool bUpdate);
	void  SetHoverObject(Object* pNewHoverObj);
	void  SetPressObject(Object* pNewPressObj);
    void  SetHoverObjectDirectNULL();
    void  SetPressObjectDirectNULL();
    void  SetFocusObjectDirect(Object* pObj);
    void  SetOriginDefaultObject(Object* pObj);
   
    void  OnObjectVisibleChangeInd(Object* pObj, bool bVisible);
    void  OnObjectRemoveInd(Object* pObj);

protected:
    long  OnMouseMove (int vkFlag, int xPos, int yPos);
    long  OnMouseLeave(int vkFlag, int xPos, int yPos);
    long  OnCancelMode(long, long );
    long  OnLButtonDown(long, long, BOOL* pbHandled);
    long  OnLButtonUp (long, long);
    long  OnRButtonDown(long,long);
    long  OnRButtonUp  (long,long);
    long  OnLButtonDBClick(long,long, BOOL* pbHandled);
    long  OnMButtonDown(long,long);
    long  OnMButtonDBClick(long,long);
    long  OnMButtonUp  (long,long);
    long  OnImeMsg(unsigned int, long, long, BOOL* pbHandled);
    void  OnKillFocus(HWND hWndFocus);
    void  OnSetFocus();
    BOOL  OnSetCursor(long,long);
    void  OnNcDestroy();
    BOOL  OnChar(long,long);
    BOOL  OnKeyDown(unsigned int nMsg, long,long);
    BOOL  OnKeyUp(long,long);
    long  OnMouseWheel(long, long);
    BOOL  OnGesture(long hGestureInfo);
    
    template <class T>
    friend void _SetHoverObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _SetPressObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _OnMouseMove(Object* pObj, long wParam, long lParam, T* pThis);
    template <class T>
    friend void _OnMouseLeave(T* pThis);
    template <class T>
    friend void _OnLButtonDown(long wParam, long lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonDBClick(long wParam, long lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonUp(long w, long l, T* pThis);

private:
    bool  AdjustDoubleClickMessage(long l);
    Object*  GetGestureTargetObject(POINT ptScreen, long wParam);

	void  updateImeStatus();

protected:
    Application*   m_pUIApplication;
    WindowBase&       m_oWindow;                  // Ҫ����Ĵ��ڶ���

    Object*  m_pObjHover;                // �Ƿ��ж������������
    Object*  m_pObjPress;                // �Ƿ��ж����ѱ�����
    Object*  m_pObjRPress;               // �Ҽ�������������ж��Ƿ���Ҫ��˭����RButtonUp��Ϣ������RDown��RUp�д��������߼��в��ܣ�Խ��Խ�ã� 
    Object*  m_pObjMPress;
    Object*  m_pObjGesture;              // �������õĶ���
    
    Object*  m_pOldFocusObject;          // ��ʱ�������ڼ�¼���㷢���仯�����еľɵĽ������
    Object*  m_pFocusObject;             // ��ǰ���ý���Ķ���

    Object*  m_pObjDefault;              // ��ǰdefault button
    Object*  m_pObjOriginDefault;        // ԭʼ��default button��û��defbtnʱ��ϵͳ����default�������õ��ö������档

    IMessage*  m_pObjKeyboardCapture;      // ��ʱ��Ҫ���ؼ�����Ϣ�Ķ���
    IMessage*  m_pObjMouseCapture;         // ��ʱ��Ҫ���������Ϣ�Ķ���
    int   m_nKeyboardCaptureNotifyMsgId;
    int   m_nMouseCaptureNotifyMsgId;

    long  m_posPrevClick;           // ���˫����λ������һ�ε�����λ�ò�һ��������Ϊ�����һ�������¼���������˫���¼�

    BOOL  m_bMouseTrack;              // �Ƿ���Ҫ�������뿪m_pWnd��
    BOOL  m_bMouseMoveReady;          // ���ڽ����������ֵĳ�������ûʹ��MOUSEMOVE��ȡhover obj�����յ���һ��BUTTONDOWN����Ϣ��
                                      // ������Ϣ��Ӧʧ�ܡ������ֶ�����һ��MOUSEMOVE���߰���ALT�����µ�MouseLeave
};

}