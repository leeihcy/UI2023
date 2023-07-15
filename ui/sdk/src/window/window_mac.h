#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_MAC_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_MAC_H_
#include "window.h"

#include <string>

#if defined(__OBJC__)
@class NSWindow;
#else
class NSWindow;
#endif

namespace ui {
class WindowPlatformMac : public WindowPlatform {
public:
  WindowPlatformMac(ui::Window& w);
  void Initialize() override;
  void Release() override;

  bool Create(const Rect &rect) override;
  void Destroy();

  void SetTitle(const char* title) override;
  void Show() override;

  void Submit(sk_sp<SkSurface> sksurface) override;

public:
  NSWindow* window() { return m_window; }
  
private:
  NSWindow *m_window = nullptr;
public:
  ui::Window& m_ui_window;
};

} // namespace ui

#endif