#ifndef _UI_SDK_SRC_WINDOW_WINDOW_MOUSE_KEY_H_
#define _UI_SDK_SRC_WINDOW_WINDOW_MOUSE_KEY_H_

namespace ui
{
class Window;

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


private:
  Window& m_window;
};

}

#endif