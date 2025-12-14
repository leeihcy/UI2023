#include "src/object/object_state.h"

#include "include/macro/msg.h"
#include "include/macro/uidefine.h"
#include "src/object/object.h"

namespace ui {

void ObjectState::SetEnable(bool b, bool bNoitfy) {
  bool bOld = IsEnable();

  SetDisableDirect(b);

  if (bNoitfy && b != bOld) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_DISABLE;
    static_cast<Object*>(this)->RouteMessage(&message);
  }

  if (b != bOld) {
    // [注] 如果没有指定刷新，则需要外部显示调用UpdateObject，因为该控件所在层
    //      并没有设置为dirty，直接刷新整个窗口也不会更新该控件
    static_cast<Object*>(this)->Invalidate();

#if 0 // defined(OS_WIN)
    // 重新发送鼠标消息，例如鼠标正好位于该控件上面，则需要将该控件设置为hover，否则点击无效
    Point pt = {0, 0};
    ::GetCursorPos(&pt);
    HWND hWnd = GetHWND();
    ::MapWindowPoints(nullptr, hWnd, &pt, 1);
    ::PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
#endif
  }
}

void ObjectState::SetDefault(bool b, bool bNotify) {
  if (m_objState.default_ == b)
    return;

  m_objState.default_ = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_DEFAULT;
    static_cast<Object*>(this)->RouteMessage(&message);
  }
}

void ObjectState::SetSelected(bool b, bool bNotify) {
  if (m_objState.selected == b)
    return;

  m_objState.selected = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_SELECTED;
    static_cast<Object*>(this)->RouteMessage(&message);
  }
}

void ObjectState::SetForceHover(bool b, bool bNotify) {
  if (m_objState.force_hover == b)
    return;

  bool bOldHover = IsHover();

  m_objState.force_hover = b;
  if (bNotify && IsHover() != bOldHover) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_HOVER;
    static_cast<Object*>(this)->RouteMessage(&message);
  }
}

void ObjectState::SetForcePress(bool b, bool bNotify) {
  if (m_objState.force_press == b)
    return;

  bool bOldPress = IsPress();

  m_objState.force_press = b;
  if (bNotify && IsPress() != bOldPress) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_PRESS;
    static_cast<Object*>(this)->RouteMessage(&message);    
  }
}

void ObjectState::SetHover(bool b, bool bNotify) {
  if (m_objState.hover == b)
    return;

  m_objState.hover = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_HOVER;
    static_cast<Object*>(this)->RouteMessage(&message);  
  }
}

void ObjectState::SetPress(bool b, bool bNotify) {
  if (m_objState.press == b)
    return;

  m_objState.press = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_PRESS;
    static_cast<Object*>(this)->RouteMessage(&message);  
  }
}
  

}