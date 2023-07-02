#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_

#include "../../base/common.h"
#include "../../base/signalslot/signal.h"

namespace ui
{
class Window;

struct WindowPlatform {
    virtual ~WindowPlatform() {};
    virtual void Initialize(ui::Window*) = 0;
    virtual void Release() = 0;
    virtual void Create(const Rect& rect) = 0;
    virtual void Show() = 0;
};

class Window {
public:
  Window();
  ~Window();

  void OnMessage();
  void Create(const Rect& rect);
  void Show();

public:
  // platform回调
  void onClose();
  void onDestroy();

  signal<void()>& SignalDestroy() { return m_signal_destroy; }

private:
  // 平台相关函数。
  WindowPlatform*  m_platform = nullptr;

  signal<void()> m_signal_destroy;
};

} // namespace ui


#endif