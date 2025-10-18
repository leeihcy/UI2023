#ifndef _UI_SDK_SRC_WINDOW_WINDOW_MOUSE_KEY_H_
#define _UI_SDK_SRC_WINDOW_WINDOW_MOUSE_KEY_H_

namespace ui
{
class Window;
class Object;
struct Point;

struct GetObjectByPosExData
{
	Object* pObjParent;
	Point*  ptParent;
	Point*  ptOutInObj;  // out

	bool  bTestDecendant;
	bool  bTestRejectMouseMsg;
	bool  bTestDisable;
};


class WindowMouseKey {
public:
  WindowMouseKey(Window& window);

  void OnMouseEnter();
  void OnMouseLeave();
  void OnMouseMove(int x, int y);
  void OnLButtonDown(int x, int y);
  void OnLButtonUp(int x, int y);
  void OnRButtonDown(int x, int y);
  void OnRButtonUp(int x, int y);

  bool OnKeyDown(int key, int flags);
  bool OnKeyUp(int key, int flags);

private:
  void _OnMouseMove(Object* pObj, Point pt);
  void _OnMouseLeave(Point pt);
  void _OnLButtonDown(Point pt);
  bool _OnLButtonDBClick(Point pt);
  void _OnLButtonUp(Point pt);

  void _SetHoverObject(Object* pNewHoverObj);
  void _SetPressObject(Object* pNewPressObj);

  void tabToPrevControl();
  void tabToNextControl();
  void checkDefPushButton(Object* pNewObj);

private:
  void updateImeStatus();

public:
  Object *GetObjectByPos(Object *pObjParent, Point *ptWindow,
                         Point *ptObj);
  Object *GetObjectByPos_UIEditor(Object *pObjParent, Point *ptWindow,
                                  Point *ptObj);
  static Object *GetObjectByPosEx(GetObjectByPosExData *pData);

  Object *GetHoverObject();
  Object *GetPressObject();
  Object *GetFocusObject();
  Object *GetDefaultObject();
  Object *GetOldFocusObject(bool bRemove = true);
  Object *GetOriginDefaultObject();

  void SetFocusObject(Object *pObj);
  void SetDefaultObject(Object *pObj, bool bUpdate);
  void SetHoverObject(Object *pNewHoverObj);
  void SetPressObject(Object *pNewPressObj);
  void SetHoverObjectDirectNULL();
  void SetPressObjectDirectNULL();
  void SetFocusObjectDirect(Object *pObj);
  void SetOriginDefaultObject(Object *pObj);

  void OnObjectVisibleChangeInd(Object *pObj, bool bVisible);
  void OnObjectRemoveInd(Object *pObj);
  void ClearStateDirect();

  void SetMouseCapture(Object*);
  void ReleaseMouseCapture(Object*);

private:
  Window &m_window;

  // 是否有对象处于鼠标下面
  Object *m_pObjHover = nullptr;
  // 是否有对象已被按下
  Object *m_pObjPress = nullptr;
  // 右键点击对象，用于判断是否需要给谁发送RButtonUp消息（仅在RDown、RUp中处理，其它逻辑中不管，越简单越好）
  Object *m_pObjRPress = nullptr;
  Object *m_pObjMPress = nullptr;
  // 手势作用的对象
  Object *m_pObjGesture = nullptr;

  // 临时对象，用于记录焦点发生变化过程中的旧的焦点对象
  Object *m_pOldFocusObject = nullptr;
  // 当前有用焦点的对象
  Object *m_pFocusObject = nullptr;

  // 当前default button
  Object *m_pObjDefault = nullptr;
  // 原始的default button，没有defbtn时，系统将把default属性设置到该对象上面。
  Object *m_pObjOriginDefault = nullptr;

  // 用于解决经常会出现的场景：还没使用MOUSEMOVE获取hover obj，就收到了一个BUTTONDOWN的消息，
  // 导致消息响应失败。例如手动发送一个MOUSEMOVE或者按下ALT键导致的MouseLeave
  bool  m_bMouseMoveReady = false;    
  
  Object*  m_pObjKeyboardCapture = nullptr;      // 临时需要拦截键盘消息的对象
  Object*  m_pObjMouseCapture = nullptr;         // 临时需要拦截鼠标消息的对象
};
}

#endif