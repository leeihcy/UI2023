#pragma once

namespace ui
{
class KeyboardMgrBase;
class WindowKeyboardMgr;
class Object;
class WindowBase;
class UIApplication;

struct GetObjectByPosExData
{
	Object* pObjParent;
	POINT*  ptParent;
	POINT*  ptOutInObj __out;  // out

	bool  bTestDecendant;
	bool  bTestRejectMouseMsg;
	bool  bTestDisable;
};

class WindowMouseMgr : public IMKMgr   // 由于目前没有将mouse/keyboard合并起来，暂时由window来实现这个接口
{
public:
    WindowMouseMgr(WindowBase& o);
    ~WindowMouseMgr(void);

    Object*  GetObjectByPos(Object* pObjParent, POINT* ptWindow, __out POINT* ptObj);
	Object*  GetObjectByPos_UIEditor(Object* pObjParent, POINT* ptWindow, __out POINT* ptObj);
    static Object*  GetObjectByPosEx(GetObjectByPosExData* pData);

public:
    UIApplication*  GetUIApplication();
    WindowBase*  GetWindowObject();
    void  SetUIApplication(UIApplication* p);

    long  HandleMessage(unsigned int msg, WPARAM w, LPARAM l, BOOL* pbHandled);
    long  HandleMouseMessage(unsigned int msg, WPARAM w, LPARAM l, BOOL* pbHandled);
    long  HandleKeyboardMessage(unsigned int msg, WPARAM w, LPARAM l, BOOL* pbHandled);
    long  HandleTouchMessage(unsigned int msg, WPARAM w, LPARAM l, BOOL& bHandled);
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
    long  OnCancelMode(WPARAM, LPARAM );
    long  OnLButtonDown(WPARAM, LPARAM, BOOL* pbHandled);
    long  OnLButtonUp (WPARAM, LPARAM);
    long  OnRButtonDown(WPARAM,LPARAM);
    long  OnRButtonUp  (WPARAM,LPARAM);
    long  OnLButtonDBClick(WPARAM,LPARAM, BOOL* pbHandled);
    long  OnMButtonDown(WPARAM,LPARAM);
    long  OnMButtonDBClick(WPARAM,LPARAM);
    long  OnMButtonUp  (WPARAM,LPARAM);
    long  OnImeMsg(unsigned int, WPARAM, LPARAM, BOOL* pbHandled);
    void  OnKillFocus(HWND hWndFocus);
    void  OnSetFocus();
    BOOL  OnSetCursor(WPARAM,LPARAM);
    void  OnNcDestroy();
    BOOL  OnChar(WPARAM,LPARAM);
    BOOL  OnKeyDown(unsigned int nMsg, WPARAM,LPARAM);
    BOOL  OnKeyUp(WPARAM,LPARAM);
    long  OnMouseWheel(WPARAM, LPARAM);
    BOOL  OnGesture(LPARAM hGestureInfo);
    
    template <class T>
    friend void _SetHoverObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _SetPressObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _OnMouseMove(Object* pObj, WPARAM wParam, LPARAM lParam, T* pThis);
    template <class T>
    friend void _OnMouseLeave(T* pThis);
    template <class T>
    friend void _OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonUp(WPARAM w, LPARAM l, T* pThis);

private:
    bool  AdjustDoubleClickMessage(LPARAM l);
    Object*  GetGestureTargetObject(POINT ptScreen, WPARAM wParam);

	void  updateImeStatus();

protected:
    UIApplication*   m_pUIApplication;
    WindowBase&       m_oWindow;                  // 要处理的窗口对象

    Object*  m_pObjHover;                // 是否有对象处于鼠标下面
    Object*  m_pObjPress;                // 是否有对象已被按下
    Object*  m_pObjRPress;               // 右键点击对象，用于判断是否需要给谁发送RButtonUp消息（仅在RDown、RUp中处理，其它逻辑中不管，越简单越好） 
    Object*  m_pObjMPress;
    Object*  m_pObjGesture;              // 手势作用的对象
    
    Object*  m_pOldFocusObject;          // 临时对象，用于记录焦点发生变化过程中的旧的焦点对象
    Object*  m_pFocusObject;             // 当前有用焦点的对象

    Object*  m_pObjDefault;              // 当前default button
    Object*  m_pObjOriginDefault;        // 原始的default button，没有defbtn时，系统将把default属性设置到该对象上面。

    IMessage*  m_pObjKeyboardCapture;      // 临时需要拦截键盘消息的对象
    IMessage*  m_pObjMouseCapture;         // 临时需要拦截鼠标消息的对象
    int   m_nKeyboardCaptureNotifyMsgId;
    int   m_nMouseCaptureNotifyMsgId;

    LPARAM  m_posPrevClick;           // 如果双击的位置与上一次单击的位置不一样，则认为这次是一个单击事件，而不是双击事件

    BOOL  m_bMouseTrack;              // 是否需要监测鼠标离开m_pWnd了
    BOOL  m_bMouseMoveReady;          // 用于解决经常会出现的场景：还没使用MOUSEMOVE获取hover obj，就收到了一个BUTTONDOWN的消息，
                                      // 导致消息响应失败。例如手动发送一个MOUSEMOVE或者按下ALT键导致的MouseLeave
};

}