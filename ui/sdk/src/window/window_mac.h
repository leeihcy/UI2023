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
  ~WindowPlatformMac();
  void Initialize() override;

  bool Create(const Rect &rect) override;
  void Destroy();

  void SetTitle(const char* title) override;
  void GetClientRect(Rect *prect) override;
  void GetWindowRect(Rect *prect) override;
  void SetWindowRect(Rect *prect);
  void Invalidate(const Rect* prect) override;
  bool IsChildWindow() override;
  bool IsWindowVisible() override;
  void Show() override;
  void Hide();
  void Commit(IRenderTarget* pRT, const Rect* prect, int count) override;

public:
  NSWindow* window() { return m_window; }
  void notifySize();
  
private:
  NSWindow *m_window = nullptr;
public:
  ui::Window& m_ui_window;
};

} // namespace ui

#endif