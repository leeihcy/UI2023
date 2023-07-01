#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_

#include "../../base/common.h"

namespace ui
{

struct WindowPlatform {
    virtual ~WindowPlatform() {};
    virtual void Initialize() = 0;
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

private:
  // 平台相关函数。
  WindowPlatform*  m_platform = nullptr;
};

} // namespace ui


#endif