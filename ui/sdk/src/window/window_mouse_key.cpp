#include "window_mouse_key.h"
#include "include/interface/icontrol.h"
#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "include/macro/vkey.h"
#include "include/macro/uidefine.h"
#include "include/util/rect.h"
#include "src/control/button/button_meta.h"
#include "src/control/button/button.h"
#include "src/render/textrender/textrender.h"
#include "window.h"
#include "src/panel/root_object.h"
#include <cstdio>

namespace ui {

WindowMouseKey::WindowMouseKey(Window &window) : m_window(window) {}

void  WindowMouseKey::ClearStateDirect()
{
	m_pObjPress = nullptr;
	m_pObjHover = nullptr;
	m_pObjRPress = nullptr;
	m_pObjMPress = nullptr;
	m_pObjGesture = nullptr;
	m_pObjDefault = nullptr;
	m_pObjOriginDefault = nullptr;
	m_pFocusObject = nullptr;
	m_pFocusObject = nullptr;
	m_pOldFocusObject = nullptr;
	m_pObjKeyboardCapture  = nullptr;
	m_pObjMouseCapture = nullptr;

	// m_bMouseTrack = TRUE;      // 默认需要进行鼠标监视
	// m_nKeyboardCaptureNotifyMsgId = 0;
	// m_nMouseCaptureNotifyMsgId = 0;
	this->m_bMouseMoveReady = false;
	// m_posPrevClick = -1;
}

Object *WindowMouseKey::GetHoverObject() { return m_pObjHover; }
Object *WindowMouseKey::GetPressObject() { return m_pObjPress; }
void WindowMouseKey::SetHoverObject(Object *pNewHoverObj) {
  _SetHoverObject(pNewHoverObj);
}
void WindowMouseKey::SetPressObject(Object *pNewPressObj) {
  _SetPressObject(pNewPressObj);
}

// 这两个Direct仅仅是用于防止崩溃的产物。应该是架构不优的问题
// 在listctrl中的内部子控件销毁时，没法通知到listctrl的mousemgr，而是去通知window的mousemgr
// 导致最后在listctrl::removeall中SetHoverObject(nullptr)中崩溃
void WindowMouseKey::SetHoverObjectDirectNULL() { m_pObjHover = nullptr; }
void WindowMouseKey::SetPressObjectDirectNULL() { m_pObjPress = nullptr; }

void WindowMouseKey::OnObjectVisibleChangeInd(Object *pObj, bool bVisible) {
  if (nullptr == pObj)
    return;

  // 即使是显示一个对象，也可能是直接显示到了当前hover object之上，如果不更新
  // hover对象，会导致再次点击时，得到响应的是老的hover object.
  // 所以显示和隐藏都要设置m_bMouseMoveReady = FALSE
  m_bMouseMoveReady = false;

  if (!bVisible) {
    if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true)) {
      SetHoverObject(nullptr);
      m_bMouseMoveReady = false;

      // GetUIApplication()->HideToolTip();
    }
    if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true)) {
      SetPressObject(nullptr);
      m_bMouseMoveReady = false;
    }

    if (m_pObjDefault == pObj) {
      SetDefaultObject(nullptr, true);
    }
    if (m_pObjOriginDefault == pObj) {
      m_pObjOriginDefault = nullptr;
    }

    // 切换新的焦点
    if (m_pFocusObject &&
        (pObj == m_pFocusObject || pObj->IsMyChild(m_pFocusObject, true))) {
      m_pFocusObject->SetFocus(false);
      // ::UISendMessage(m_pFocusObject, WM_KILLFOCUS, (WPARAM)pObj, 0);
      m_pFocusObject = nullptr;

      this->tabToNextControl();
    }
  }
}

void WindowMouseKey::OnObjectRemoveInd(Object *pObj) {
  if (nullptr == pObj)
    return;

  if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true)) {
    m_pObjHover = nullptr;
    m_bMouseMoveReady = false;
  }
  if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true)) {
    m_pObjPress = nullptr;
    m_bMouseMoveReady = false;
  }
  if (m_pObjRPress == pObj || pObj->IsMyChild(m_pObjRPress, true)) {
    m_pObjRPress = nullptr;
  }
  if (m_pObjMPress == pObj || pObj->IsMyChild(m_pObjMPress, true)) {
    m_pObjMPress = nullptr;
  }
  if (m_pObjGesture == pObj || pObj->IsMyChild(m_pObjGesture, true)) {
    m_pObjGesture = nullptr;
  }
  if (m_pObjDefault == pObj || pObj->IsMyChild(m_pObjDefault, true)) {
    m_pObjDefault = nullptr;
  }
  if (m_pObjOriginDefault == pObj ||
      pObj->IsMyChild(m_pObjOriginDefault, true)) {
    m_pObjOriginDefault = nullptr;
  }

  if (pObj == m_pFocusObject || pObj->IsMyChild(m_pFocusObject, true)) {
    m_pFocusObject = nullptr;
  }
  if (pObj == m_pOldFocusObject) {
    m_pOldFocusObject = nullptr;
  }

  if (pObj == m_pObjKeyboardCapture) {
    m_pObjKeyboardCapture = nullptr;
  }
  if (pObj == m_pObjMouseCapture) {
    m_pObjMouseCapture = nullptr;
  }
}

//
//	[递归+遍历] [public] [static]
//
//	获取当前鼠标下的最小单位控件. （
//不可见，不可用，不接受鼠标消息的控件、Panel 这些类型的对象不返回）
//
//	Parameter
//
//		pObjParent
//			[in]
//				递归遍历时，使用的parent
//object.调用该函数时赋值为窗口对象(Window/)pObj即可。 		ptParent 			[in]
//				在父对象中的鼠标位置，初始值为在窗口中的位置
//
//      bUIEditorInvoke
//          [in]
//              在UIBuilder中只需要一个完整的CONTROL例如不查找SliderCtrl中的button，
//              另外UIBuilder还需要查找不接受鼠标消息类型的控件，如Label,
//              Picture
//
//	Return
//		该位置下的对象，没有则返回空
//
Object *WindowMouseKey::GetObjectByPos(Object *pObjParent, Point *ptParent,
                                       Point *ptOutInObj) {
  GetObjectByPosExData data = {0};
  data.pObjParent = pObjParent;
  data.ptParent = ptParent;
  data.ptOutInObj = ptOutInObj;
  data.bTestDecendant = true;
  data.bTestRejectMouseMsg = true;
  data.bTestDisable = false;
  return GetObjectByPosEx(&data);
}

Object *WindowMouseKey::GetObjectByPos_UIEditor(Object *pObjParent,
                                                Point *ptParent,
                                                Point *ptOutInObj) {
  GetObjectByPosExData data = {0};
  data.pObjParent = pObjParent;
  data.ptParent = ptParent;
  data.ptOutInObj = ptOutInObj;
  data.bTestDecendant = true;
  data.bTestRejectMouseMsg = false;
  data.bTestDisable = false;
  return GetObjectByPosEx(&data);
}

//  将这个不放在GetObjectByPos中，避免影响GetObjectByPos的效率
//  注:
//  鼠标检测到一个Panel时，并且没有命中该panel的子结点时，应该结点去检测panel的
//        下一个结点，而不是直接返回Null。
Object *WindowMouseKey::GetObjectByPosEx(GetObjectByPosExData *pData) {
  Object *pObjParent = pData->pObjParent;
  Point *ptParent = pData->ptParent;
  Point *ptOutInObj = pData->ptOutInObj;

  Object *pHitObj = nullptr;
  Point ptHitTest = *ptParent;

  Object *pChild = nullptr;

  bool bEnumNcChinding = true;
  while ((pChild = pObjParent->REnumAllChildObject(pChild))) {
    if (bEnumNcChinding && !pChild->IsNcObject()) {
      bEnumNcChinding = false;

      // 开始枚举客户区对象，纠正偏移量，再继续搜索客户区的子对象
      REGION4 rcNonClient = {0};
      pObjParent->GetNonClientRegion(&rcNonClient);

      ptHitTest.x -= rcNonClient.left;
      ptHitTest.y -= rcNonClient.top;

      Point ptOffset = {0, 0};
      if (pObjParent->GetScrollOffset((int *)&ptOffset.x, (int *)&ptOffset.y)) {
        ptHitTest.x += ptOffset.x;
        ptHitTest.y += ptOffset.y;
      }
    }

    if (!pChild->IsVisible())
      continue;

    if (pData->bTestDisable) {
      if (!pChild->IsEnable()) // 注：disable控件应该允许tooltips
        continue;
    }

    if (pData->bTestRejectMouseMsg) {
      if (pChild->IsRejectMouseMsgAll())
        continue;
    }

    HitTestMessage hittest;
    hittest.pt_in_parent = ptHitTest;
    pChild->RouteMessage(&hittest);
    if (eHitTest::Nowhere == hittest.hittest)
      continue;

    pHitObj = pChild;

    if (pData->bTestDecendant) {
      Point ptInHitChild = {0};

      GetObjectByPosExData data = {0};
      data.pObjParent = pHitObj;
      data.ptParent = &hittest.pt_in_child;
      data.ptOutInObj = &ptInHitChild;
      data.bTestDecendant = pData->bTestDecendant;
      data.bTestDisable = pData->bTestDisable;
      data.bTestRejectMouseMsg = pData->bTestRejectMouseMsg;
      Object *pRetObj = GetObjectByPosEx(&data);

      // 若子控件中没有处于该位置的，直接返回本对象
      if (pRetObj) {
        if (ptOutInObj)
          *ptOutInObj = ptInHitChild;
        return pRetObj;
      }
    }

    if (pHitObj && pData->bTestRejectMouseMsg) {
      if (pHitObj->IsRejectMouseMsgSelf() || pHitObj->IsRejectMouseMsgAll()) {
        pHitObj = nullptr;
        continue;
      }
    }

    if (pHitObj && ptOutInObj)
      *ptOutInObj = hittest.pt_in_child;

    break;
  }

  return pHitObj;
}

void WindowMouseKey::OnMouseEnter() {}
void WindowMouseKey::OnMouseLeave() {}

void WindowMouseKey::OnMouseMove(int x, int y) {
  if (m_pObjMouseCapture) {
    MouseMoveMessage msg;
    msg.pt_in_window = {x, y};
    m_pObjMouseCapture->RouteMessage(&msg);
    return;
  }

  // if (m_bMouseTrack) //若允许追踪，则。
  // {
  //   TRACKMOUSEEVENT tme;
  //   tme.cbSize = sizeof(TRACKMOUSEEVENT);
  //   tme.dwFlags = TME_LEAVE;
  //   tme.dwHoverTime = HOVER_DEFAULT;
  //   tme.hwndTrack = this->m_oWindow.GetHWND();
  //   ::TrackMouseEvent(&tme);
  //   m_bMouseTrack = FALSE;
  // }

  Object *pOldHover = m_pObjHover;
  Object *pOldPress = m_pObjPress;

  this->m_bMouseMoveReady = true;

	// 1. 判断当前鼠标位置
	Point pt = { x, y };
  Point ptInObj = {0};

	Object* pObj = this->GetObjectByPos(&m_window.GetRootObject(), &pt, &ptInObj);

	_OnMouseMove(pObj, pt);

  if (pOldPress == m_pObjPress &&
      pOldHover != m_pObjHover) // 鼠标下的对象发生变化，需要重置鼠标样式
  {
    // HWND hWnd = m_oWindow.GetHWND();
    // if (GetCapture() != hWnd) // 鼠标拖拽时不变，例如从EDIT中拖拽出来
    // {
    //   ::PostMessage(hWnd, WM_SETCURSOR, (WPARAM)hWnd,
    //                 MAKELPARAM(HTCLIENT, 1)); // hiword 0表示弹出菜单
    // }
  }
}

void WindowMouseKey::_OnMouseMove(Object *pObj, Point pt) {
  // 将所有的情况都列出来进行判断
  if (nullptr == m_pObjPress) {
    if (nullptr == m_pObjHover) {
      // 鼠标还是在外面移动
      if (pObj == m_pObjHover) {
      }
      // 鼠标移动到了pObj上
      else {
        SetHoverObject(pObj);

        MouseMoveMessage msg;
        msg.pt_in_window = pt;
        m_pObjHover->RouteMessage(&msg);
      }
    } else {
      // 鼠标在A上面移动
      if (pObj == m_pObjHover) {
        MouseMoveMessage msg;
        msg.pt_in_window = pt;
        m_pObjHover->RouteMessage(&msg);
      }
      // 鼠标刚才放在A上面，但现在又移出去了，可能移到B上面，也可能没有移动任何对象上
      else {
        Object *pSaveHover = m_pObjHover;
        SetHoverObject(pObj); // 先设置状态，再发送消息

        MouseLeaveMessage msg;
        msg.pt_in_window = pt;
        pSaveHover->RouteMessage(&msg);
        if (pObj) {
          MouseMoveMessage msg;
          msg.pt_in_window = pt;
          pObj->RouteMessage(&msg);
        }
      }
    }
  } else {
    if (nullptr == m_pObjHover) {
      // 鼠标按在了A对象上，但鼠标在A外面。现在又移动回来了
      if (pObj == m_pObjPress) {
        SetHoverObject(m_pObjPress);

        MouseMoveMessage msg;
        msg.pt_in_window = pt;
        m_pObjPress->RouteMessage(&msg);
      }
      // 鼠标在A对象上按下了，但鼠标现在不再A上面
      else {
        MouseMoveMessage msg;
        msg.pt_in_window = pt;
        m_pObjPress->RouteMessage(&msg);
      }
    } else {
      // 鼠标按在了A对象上，现在在A上移动
      if (pObj == m_pObjPress) {
        UIASSERT(m_pObjPress == m_pObjHover);
        UIASSERT(m_pObjHover == pObj);

        MouseMoveMessage msg;
        msg.pt_in_window = pt;
        m_pObjPress->RouteMessage(&msg);
      }
      // 鼠标刚才按在了A对象上，但现在鼠标移出来了。
      else {
        SetHoverObject(nullptr);

        MouseMoveMessage msg;
        msg.pt_in_window = pt;
        m_pObjPress->RouteMessage(&msg);
      }
    }
  }
}

void WindowMouseKey::_OnMouseLeave(Point pt) {
  // 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
  Object *pSaveObjPress = m_pObjPress;
  Object *pSaveObjHover = m_pObjHover;

  SetHoverObject(nullptr);
  SetPressObject(nullptr);

  if (pSaveObjHover) {
    MouseLeaveMessage msg;
    msg.pt_in_window = pt;
    pSaveObjHover->RouteMessage(&msg);
  }

  if (pSaveObjPress && pSaveObjPress != pSaveObjHover) {
    MouseLeaveMessage msg;
    msg.pt_in_window = pt;
    pSaveObjPress->RouteMessage(&msg);
  }
}

void WindowMouseKey::OnLButtonDown(int x, int y) {
  if (m_pObjMouseCapture) {
    LButtonDownMessage msg;
    msg.pt_in_window = {x, y};
    m_pObjMouseCapture->RouteMessage(&msg);
    return;
  }

  if (m_pObjHover) {
    checkDefPushButton(m_pObjHover);
  }

  // TODO: 事件冒泡逻辑？
  // LButtonDownEvent event;
  // event.obj = m_window.GetRootObject().GetIObject();
  // event.x = x;
  // event.y = y;
  // m_window.emit(LBUTTON_DOWN_EVENT, &event);

  // 保存本次点击位于，用于过滤双击事件
  // m_posPrevClick = l;

  if (!this->m_bMouseMoveReady) {
    OnMouseMove(x, y);
  }

  // if (m_pObjHover) {
  //   ::SetCapture(this->m_oWindow.GetHWND());
  // }

  _OnLButtonDown({x, y});
}

void WindowMouseKey::_OnLButtonDown(Point pt) {
  if (m_pObjHover && m_pObjHover->IsEnable()) {
    SetPressObject(m_pObjHover);

    Object *pSave = m_pObjPress;

    // 应该是先setfocus，再lbuttondown/up
    if (m_pObjPress && m_pObjPress->CanTabstop()) {
      SetFocusObject(m_pObjPress);
    }

    if (m_pObjPress && pSave == m_pObjPress) {
      LButtonDownMessage msg;
      msg.pt_in_window = pt;
      m_pObjHover->RouteMessage(&msg); // 有可能导致m_pObjPress为nullptr了
    }
  }
}

bool WindowMouseKey::_OnLButtonDBClick(Point pt) {
  bool bHandled = false;
  if (m_pObjPress) {
    LButtonDBClickMessage msg;
    msg.pt_in_window = pt;
    m_pObjPress->RouteMessage(&msg);

    if (m_pObjPress && m_pObjPress->IsTabstop()) {
      SetFocusObject(m_pObjPress);
    }
    bHandled = true;
  } else if (m_pObjHover && m_pObjHover->IsEnable()) {
    SetPressObject(m_pObjHover);
    LButtonDBClickMessage msg;
    msg.pt_in_window = pt;
    m_pObjHover->RouteMessage(&msg);

    if (m_pObjPress && m_pObjPress->IsTabstop()) {
      SetFocusObject(m_pObjPress);
    }
    bHandled = true;
  }

  return bHandled;
}

void WindowMouseKey::OnLButtonUp(int x, int y) {
  if (m_pObjMouseCapture) {
    LButtonUpMessage msg;
    msg.pt_in_window = {x, y};
    m_pObjMouseCapture->RouteMessage(&msg);
    return;
  }

  // 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
  Object *pSaveObjPress = m_pObjPress;
  Object *pSaveObjHover = m_pObjHover;

  // 先释放capture。up的响应函数中（如按钮click）可能弹出
  // 菜单，进入阻塞模式。但capture却一直还是按钮窗口上面。
  //
  // 并且不能放在if (m_pObjPress)中。m_pObjPress有可能中途被清空
#if 0
  if (GetCapture() == m_oWindow.GetHWND())
    ::ReleaseCapture();
#endif
  if (m_pObjPress) {
    _OnLButtonUp({x, y});

#if 0 // TODO:
    // 如果这个时候鼠标离开了控件，发送MOUSELEAVE消息.
    // 这个时候鼠标位置可能已经重新变化了，需要重新获取
    Point ptCursorNow;
    GetCursorPos(&ptCursorNow);
    ::MapWindowPoints(nullptr, m_oWindow.GetHWND(), &ptCursorNow, 1);

    Object *pNowHover = this->GetObjectByPos(&m_oWindow, &ptCursorNow, nullptr);
    this->SetHoverObject(pNowHover);
    if (pNowHover != pSaveObjPress && nullptr != pSaveObjPress)
      ::UISendMessage(pSaveObjPress, WM_MOUSELEAVE);
    if (pNowHover != pSaveObjHover && nullptr != pNowHover)
      ::UISendMessage(pNowHover, WM_MOUSEMOVE, w, l);
#endif
  }
}

void WindowMouseKey::_OnLButtonUp(Point pt) {
  if (m_pObjPress) {
    if (m_pObjPress) {
      Object *pSavePress = m_pObjPress;

      LButtonUpMessage msg;
      msg.pt_in_window = pt;
      pSavePress->RouteMessage(&msg);

      // SetPressObject(nullptr)放在UISendMessage前面，
      // 会导致checkbutton点击后不刷新
      if (m_pObjPress == pSavePress)
        SetPressObject(nullptr);
      else
        pSavePress->SetPress(false);
    }
  }
}

void WindowMouseKey::OnRButtonDown(int x, int y) {}

void WindowMouseKey::OnRButtonUp(int x, int y) {}

void WindowMouseKey::_SetHoverObject(Object *pNewHoverObj) {
  Object *pOldHoverObj = m_pObjHover;
  if (m_pObjHover == pNewHoverObj)
    return;

  // Application &app = m_window.GetApplication();
  // app.HideToolTip();
  // if (nullptr != pNewHoverObj) {
  //   TOOLTIPITEM item = {0};
  //   item.pNotifyObj = pNewHoverObj->GetIObject();
  //   app.ShowToolTip(&item);
  // }

  // 注意：
  // 先改变所有对象的状态，再发送消息，避免在第一个对象响应消息的时候拿不到第二个对象的状态
  if (m_pObjHover) {
    m_pObjHover->SetHover(false, false);
  }

  // diable的object仅可以弹出tooltip，不能设置为hover
  if (pNewHoverObj && pNewHoverObj->IsEnable()) {
    pNewHoverObj->SetHover(true, false);
  }

  m_pObjHover = pNewHoverObj;

  StateChangedMessage msg;
  msg.state_changed_mask = OSB_HOVER;

  if (pOldHoverObj) {
    pOldHoverObj->RouteMessage(&msg);
  }
  if (pNewHoverObj) {
    pNewHoverObj->RouteMessage(&msg);
  }
}

void WindowMouseKey::_SetPressObject(Object *pNewPressObj) {
  if (m_pObjPress == pNewPressObj)
    return;

  Object *pOldPressObj = m_pObjPress;
  if (m_pObjPress) {
    m_pObjPress->SetPress(false, false);
  }
  if (pNewPressObj) {
    pNewPressObj->SetPress(true, false);
  }

  // Application &app = m_window.GetApplication();
  // app.HideToolTip();

  m_pObjPress = pNewPressObj;

  StateChangedMessage msg;
  msg.state_changed_mask = OSB_PRESS;

  if (pOldPressObj) {
    pOldPressObj->RouteMessage(&msg);
  }
  if (pNewPressObj) {
    pNewPressObj->RouteMessage(&msg);
  }
}


#if 0

// 该函数同时还支撑着WM_CANCELMODE的作用
void WindowMouseMgr::OnKillFocus(HWND hFocusWnd)
{
	if (this->m_pObjHover)
	{
		::UISendMessage(m_pObjHover, WM_MOUSELEAVE);
	}
	if (this->m_pObjPress)
	{
        HWND hWndCapture = GetCapture();
        HWND hWnd = m_oWindow.GetHWND();
        if (hWndCapture == hWnd) 
        {
            ::ReleaseCapture();
        }

		if (this->m_pObjPress != this->m_pObjHover)  // 避免将WM_MOUSELEAVE发送两次
			::UISendMessage(m_pObjPress, WM_MOUSELEAVE);
	}
    else
    {
        HWND hWndCapture = GetCapture();
        if (hWndCapture) 
        {
            // 子窗口正setcapture，然后外部调用EnableWindow(主窗口,FALSE)
            // 将导致主窗口收到killfocus消息。子窗口的capture没有被release
            HWND hWnd = m_oWindow.GetHWND();
            if (hWndCapture == hWnd || ::IsChild(hWnd, hWndCapture))
            {
                ::ReleaseCapture();
            }
        }
    }

	this->SetHoverObject(nullptr);
	this->SetPressObject(nullptr);
    this->m_bMouseMoveReady = FALSE;  // 失焦点后直接触发鼠标按下会导致状态不正确

    m_pObjKeyboardCapture = nullptr;
    m_nKeyboardCaptureNotifyMsgId = 0;
    m_pObjMouseCapture = nullptr;
    m_nMouseCaptureNotifyMsgId = 0;

    // TODO: KEYBOARD
    Object* p = this->GetOldFocusObject();
    if (nullptr == p)
        p = m_pFocusObject;

    if (p)  // 给Focus对象发送焦点消息，但不重置m_pFocusObject。例如鼠标点到桌面上导致的失焦
    {
        p->SetFocus(false);
        ::UISendMessage(p, WM_KILLFOCUS, (WPARAM)nullptr, (LPARAM)0);
    }

    // 当鼠标是点在了本窗口上面的一个HOSTWND里的子窗口时，重置m_pFocusObject，否则鼠标再点回m_pFocusObject时将没反应
    if (m_oWindow)
    {
        HWND hWnd = m_oWindow.GetHWND();
        if (IsChild(hWnd, hFocusWnd))
            this->SetFocusObjectDirect(nullptr);
    }
}

void WindowMouseMgr::OnSetFocus()
{
    if (m_pFocusObject && !m_pFocusObject->IsFocus())
    {
        m_pFocusObject->SetFocus(true);
        ::UISendMessage(m_pFocusObject, WM_SETFOCUS, (WPARAM)GetOldFocusObject(), (LPARAM)0);  // 这里的GetOldFocusObject对应于
    }
    else
    {
        this->Tab_2_NextControl();
    }
	updateImeStatus();
}

BOOL WindowMouseMgr::OnSetCursor( WPARAM w,LPARAM l )
{
    Object* pObj = m_pObjPress;
    if (nullptr == pObj)
        pObj = m_pObjHover;

    BOOL bHandled = FALSE;
	if (pObj)
	{
		::UISendMessage(pObj->GetIMessage(), WM_SETCURSOR, w, l, 0, 0, 0, &bHandled);
	}
	return bHandled;
}
#endif

Object *WindowMouseKey::GetOldFocusObject(bool bRemove) {
  if (bRemove) {
    Object *p = m_pOldFocusObject;
    m_pOldFocusObject = nullptr;
    return p;
  } else {
    return m_pOldFocusObject;
  }
}

void WindowMouseKey::SetFocusObjectDirect(Object *pObj) {
  m_pFocusObject = pObj;
}

Object *WindowMouseKey::GetFocusObject() { return m_pFocusObject; }

void WindowMouseKey::SetDefaultObject(Button *pObj, bool bUpdate) {
  if (pObj == m_pObjDefault)
    return;

  if (m_pObjDefault)
    m_pObjDefault->SetDefault(false, bUpdate);

  m_pObjDefault = pObj;

  if (m_pObjDefault)
    m_pObjDefault->SetDefault(true, bUpdate);
}

Button *WindowMouseKey::GetOriginDefaultObject() { return m_pObjOriginDefault; }
void WindowMouseKey::SetOriginDefaultObject(Button *pObj) {
  m_pObjOriginDefault = pObj;
  if (m_pObjOriginDefault && nullptr == m_pObjDefault) {
    SetDefaultObject(m_pObjOriginDefault, true);
  }
}
Object *WindowMouseKey::GetDefaultObject() { return m_pObjDefault; }


//
//	该函数只会在两个地方触发:
//		a. 鼠标点击选中一个UI控件
//		b. 使用TAB切换控件
//
//	因此当鼠标点击选中一个系统控件时，需要在HWNDHOST中调用一次SetFocusObjectDirect
//
void WindowMouseKey::SetFocusObject(Object *pObj) {
  // TODO: 这个条件会导致焦点从系统的combobox.edit点击到ui combobox.edit时没反应
  //       但屏蔽这个条件会导致UI
  //       COMBOBOX中的EDIT无法拖拽选中（因为一直在KILLFOCUS/SETFOCUS）
  if (m_pFocusObject == pObj)
    return;

#if 0
  if (pObj) {
    if (pObj->GetDescription() &&
        OBJ_HWNDHOST == pObj->GetDescription()->GetMajorType()) {
      HWND hWndFocus = ::GetFocus();
      HWND hWndHost = ((HwndHost *)pObj)->m_hWnd;

      // 有可能点击了combobox中的edit,edit不是一个HOSTWND，但是HOSTWND的子窗口
      if (hWndFocus != hWndHost && FALSE == ::IsChild(hWndHost, hWndFocus)) {
        // 在HwndHost::WndProc WM_SETFOCUS中使用
        m_pOldFocusObject = m_pFocusObject;
        m_pFocusObject = pObj;

        if (GetWindowLongPtr(hWndHost, GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
        } else
          ::SetFocus(hWndHost);
        return;
      }
    } else {
      if (m_oWindow && ::GetFocus() != m_oWindow.GetHWND()) {
        m_pOldFocusObject = m_pFocusObject; // 在KeyboardManager::SetFocus中使用
        m_pFocusObject = pObj;

        HWND hWnd = m_oWindow.GetHWND();
        if (!IsWindowVisible(hWnd) ||
            GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) {

        } else {
          ::SetFocus(hWnd);
        }

        return;
      }
    }
  }
#endif

  if (m_pFocusObject) {
    m_pFocusObject->SetFocus(false);
    KillFocusMessage msg;
    if (pObj) {
      msg.new_focus = pObj->GetIObject();
    }
    m_pFocusObject->RouteMessage(&msg);
  }
  if (pObj) {
    pObj->SetFocus(true);

    SetFocusMessage msg;
    if (m_pFocusObject) {
    msg.old_focus = m_pFocusObject->GetIObject();
    }
    pObj->RouteMessage(&msg);
  }
  m_pFocusObject = pObj;

  updateImeStatus();
}


void  WindowMouseKey::updateImeStatus()
{
#if 0
	// 根据焦点对象切换输入法状态
	// 窗口的输入法，用于区分一个控件是否需要启用输入法。
	// 如果一个控件不启用输入法，即使切换到中文输入法，也不会显示输入法的状态栏的。
	OBJSTYLE s = {0};
	s.enable_ime = 1;
	if (m_pFocusObject && m_pFocusObject->TestObjectStyle(s))
	{
		//ImmAssociateContext(m_oWindow.m_hWnd, m_hIMC);
		ImmAssociateContextEx(m_oWindow.m_hWnd, nullptr, IACE_DEFAULT);
	}
	else
	{
		ImmAssociateContextEx(m_oWindow.m_hWnd, nullptr, IACE_IGNORENOCONTEXT);
		//ImmAssociateContext(m_oWindow.m_hWnd, nullptr);
	}
#endif
}

bool WindowMouseKey::OnKeyDown(int key, int flags) {
  if (isDialogKeyDownMessage(key, flags)) {
    return true;
  }

  if (this->m_pFocusObject) {
    KeyDownMessage msg;
    msg.key = key;
    msg.flags = flags;
    m_pFocusObject->RouteMessage(&msg);
    return true;
  }

  return false;
}

bool WindowMouseKey::OnKeyUp(int key, int flags) {
  if (this->m_pFocusObject) {
    KeyUpMessage msg;
    msg.key = key;
    msg.flags = flags;
    m_pFocusObject->RouteMessage(&msg);
    return true;
  }

  return false;
}

// TAB、RETURN 两个按键逻辑处理，切换Focus、Default Push Button
bool WindowMouseKey::isDialogKeyDownMessage(int key, int flags) {
  if (VKEY_TAB == key) {
#if 0 // TODO:
    if (!IsWindowEnabled(m_oWindow.m_hWnd))
      return false;
#endif

#if defined(OS_WIN)
    // LRESULT lRet = SendMessage(m_oWindow.m_hWnd, WM_QUERYUISTATE, 0, 0);
    // if (lRet & UISF_HIDEFOCUS) {
    //   lRet &= ~UISF_HIDEFOCUS;
    //   SendMessage(m_oWindow.m_hWnd, WM_UPDATEUISTATE, lRet | UIS_SET, 0);
    // }
#endif

    OBJSTYLE style = {0};
    style.want_tab = 1;
    if (m_pFocusObject && m_pFocusObject->TestObjectStyle(style)) {
        return false;
    }

    // 导航
    if (flags & VKEY_FLAG_SHIFT) {
      tabToPrevControl();
    } else {
      tabToNextControl();
    }
    return true;
  } else if (VKEY_RETURN == key || VKEY_EXECUTE == key) {
    OBJSTYLE style;
    style.want_return = 1;
    if (m_pFocusObject && m_pFocusObject->TestObjectStyle(style)) {
      // 例如在多行编辑框中回车
      return false;
    }

    // Windows的做法是给窗口发送一个WM_COMMAND( id, hwnd )的消息
    // 查找default button，发送给defpushbutton
    if (m_pObjDefault && m_pObjDefault->IsEnable() &&
        m_pObjDefault->IsVisible()) {
      DefaultButtonReturnMessage msg;
      m_pObjDefault->RouteMessage(&msg);
      return true;
    }
  }
  return false;
}

#if 0

BOOL WindowMouseMgr::OnChar(WPARAM w, LPARAM l)
{	
    if (this->m_pFocusObject)
    {
        UISendMessage(m_pFocusObject, WM_CHAR, w, l);
        return TRUE;
    }
    return FALSE;
}
long WindowMouseMgr::OnMouseWheel(WPARAM w, LPARAM l)
{
    // 先询问当前的press obj，能否进行mouse wheel分发
    if (m_pObjPress)
    {
        BOOL  bHandled = (BOOL)UISendMessage(m_pObjPress->GetIMessage(), UI_MSG_MOUSEWHEEL_REQ, w, l);
        if (bHandled)
            return 0;
    }

    // 先交给鼠标下的hover对象处理
    if (m_pObjHover)
    {
        BOOL  bHandled = (BOOL)UISendMessage(m_pObjHover->GetIMessage(), WM_MOUSEWHEEL, w, l);
        if (bHandled)
            return 0;
    }

    // 如果焦点对象没处理，则抛给焦点对象的父对象。
    // 例如一个位于ScrollPanel中的Edit，在Edit
    // 中进行MouseWheel的话，则应该滚动ScrollPanel

    Object*  pObj = m_pFocusObject;
	if (!pObj)
	{
		// 没有focus时，直接给hover对象进行冒泡
		if (m_pObjHover)
			pObj = m_pObjHover->GetParentObject();
	}

    while (pObj)
    {
        BOOL  bHandled = (BOOL)UISendMessage(pObj->GetIMessage(), WM_MOUSEWHEEL, w, l);
        if (bHandled)
            break;

        pObj = pObj->GetParentObject();
    }
    return 0;
}

LRESULT  WindowMouseMgr::OnImeMsg(UINT uMsg, WPARAM w, LPARAM l, BOOL* pbHandled)
{
    if (this->m_pFocusObject)
    {
        return UISendMessage(m_pFocusObject->GetIMessage(), uMsg, w, l, 0, nullptr, 0, pbHandled);
    }
    return 0;
}

// 系统发出来的双击事件是基于窗口客户区域的。
// 但在dui中，每个控件都是无窗口的，有可能在控件A上单击了下，然后立即
// 跑到控件B上双击了。因此对双击事件做一个过滤。
//
// return: true是一个双击事件, false做为单击事件处理
bool  WindowMouseMgr::AdjustDoubleClickMessage(LPARAM l)
{
    if (m_posPrevClick == l)
        return true;

    return false;
}

#endif

void WindowMouseKey::tabToNextControl() {
  Object *p = m_pFocusObject;
  if (!p)
    p = &m_window.GetRootObject();

  if (!p)
    return;

  p = p->GetNextTabObject();

  if (p) {
    checkDefPushButton(p);
    SetFocusObject(p);
  }
}
void WindowMouseKey::tabToPrevControl() {
  Object *p = m_pFocusObject;
  if (nullptr == p) {
    p = &m_window.GetRootObject();
  }
  p = p->GetPrevTabObject();

  if (p) {
    checkDefPushButton(p);
    SetFocusObject(p);
  }
}

/*
Code                   Meaning
------------------------------------------------------------------------

DLGC_BUTTON            Control is a button (of any kind).
DLGC_DEFPUSHBUTTON     Control is a default push button.
DLGC_HASSETSEL         Windows will send an EM_SETSEL message to the control to select its contents.
DLGC_RADIOBUTTON       Control is an option (radio) button.
DLGC_STATIC            Control is a static control.
DLGC_UNDEFPUSHBUTTON   Control is a push button but not the default push button.(是一个按钮，但不是default)
DLGC_WANTALLKEYS       Control processes all keyboard input.
DLGC_WANTARROWS        Control processes arrow keys.
DLGC_WANTCHARS         Control processes WM_CHAR messages.
DLGC_WANTMESSAGE       Control processes the message in the MSG structure that lParam points to.
DLGC_WANTTAB           Control processes the TAB key.
*/

void WindowMouseKey::checkDefPushButton(Object *pNewObj) {
  if (nullptr == pNewObj || m_pObjDefault == pNewObj) {
    return;
  }

  IButton *p = (IButton *)pNewObj->QueryInterface(IButton::UUID());
  if (!p) {
    return;
  }
  Button *new_button = p->GetImpl();

  ButtonStyle button_style = {0};
  button_style.auto_default = 1;
  bool new_button_auto_default = new_button->IsAutoDefault();

  if (pNewObj == m_pFocusObject) {
    if (new_button_auto_default) {
      SetDefaultObject(new_button, true);
    }
    return;
  }

  Button *pLastDefaultObj = m_pObjDefault; // 用于计算最终defbtn

  /*
   * If the focus is changing to or from a pushbutton, then remove the
   * default style from the current default button
   */
  bool focus_auto_default = false;
  if (m_pFocusObject) {
    IButton* focus_button = (IButton*)m_pFocusObject->QueryInterface(IButton::UUID());
    if (focus_button) {
      focus_auto_default = focus_button->GetImpl()->IsAutoDefault();
    }
  }
  if (focus_auto_default || new_button_auto_default) {
    pLastDefaultObj = nullptr;
  }

  if (new_button_auto_default) {
    pLastDefaultObj = new_button;
  } else {
    pLastDefaultObj = m_pObjOriginDefault;
  }

  SetDefaultObject(pLastDefaultObj, true);
}

void WindowMouseKey::SetMouseCapture(Object *obj) { m_pObjMouseCapture = obj; }
void WindowMouseKey::ReleaseMouseCapture(Object *obj) {
  if (obj == m_pObjMouseCapture) {
    m_pObjMouseCapture = nullptr;
  }
}

} // namespace ui
