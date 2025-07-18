#include "window_mouse_key.h"
#include "window.h"
#include "src/panel/root_object.h"

namespace ui {

WindowMouseKey::WindowMouseKey(Window &window) : m_window(window) {}

void WindowMouseKey::OnMouseEnter() {}
void WindowMouseKey::OnMouseLeave() {}

void WindowMouseKey::OnMouseMove(int x, int y) {}

void WindowMouseKey::OnLButtonDown(int x, int y) {
  // TODO: 事件冒泡逻辑？
  LButtonDownEvent event;
  event.obj = m_window.GetRootObject().GetIObject();
  event.x = x;
  event.y = y;
  m_window.emit(LBUTTON_DOWN_EVENT, &event);
}

void WindowMouseKey::OnLButtonUp(int x, int y) {}

void WindowMouseKey::OnRButtonDown(int x, int y) {}

void WindowMouseKey::OnRButtonUp(int x, int y) {}

} // namespace ui
