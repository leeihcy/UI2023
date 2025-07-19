#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_MAC_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_MAC_H_
#include "gpu/include/api.h"
#include "window.h"

#include <string>

#if defined(__OBJC__)
@class NSWindow;
#else
class NSWindow;
#endif

namespace ui {

//
// 1. macOS中，(0,0)坐标对应于屏幕左下角。
// 2. macOS 使用 Points（逻辑坐标）而非 Pixels（物理像素）布局 UI：
//    1 Point = 1 Pixel（非 Retina）
//    1 Point = 2×2 Pixels（Retina）

class WindowPlatformMac : public WindowPlatform, public IGpuCompositorWindowNSView {
public:
  WindowPlatformMac(ui::Window& w);
  ~WindowPlatformMac();

  IGpuCompositorWindow* GetGpuCompositorWindow() override {
    return static_cast<IGpuCompositorWindow*>(this);
  }
  void Initialize() override;

  bool Create(CreateWindowParam& param) override;
  void PostCreate() override;

  void Destroy();
  WINDOW_HANDLE GetWindowHandle() override;
  WINDOW_HANDLE GetRootViewHandle();

  void SetTitle(const char* title) override;
  void GetClientRect(Rect *prect) override;
  void GetWindowRect(Rect *prect) override;
  void UpdateNonClientRegion(Rect* pregion) override;
  void GetMonitorWorkArea(Rect* rect) override;
  void SetWindowPos(int x, int y, int w, int h, SetPositionFlags flags) override;
  void SetWindowRect(Rect *prect);
  // void Invalidate(const Rect* prect) override;
  bool IsChildWindow() override;
  bool IsWindowVisible() override;
  void Show() override;
  void Hide();
  float GetScaleFactor() override;
  void Commit(IRenderTarget* pRT, const Rect* prect, int count) override;

protected:
  // IGpuCompositorWindowNSView
  void* GetNSWindowRootView() override;

public:
  NSWindow* window() { return m_window; }
  void notifySize();
  void onPaint(const Rect& dirty);

private:
  NSWindow *m_window = nullptr;
public:
  ui::Window& m_ui_window;
};

} // namespace ui

#endif