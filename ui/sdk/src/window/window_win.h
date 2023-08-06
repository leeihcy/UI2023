#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_WIN_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_WIN_H_
#include "window.h"

#include <string>

namespace ui {
class WindowPlatformWin : public WindowPlatform {
public:
  WindowPlatformWin(ui::Window &w);
  void Initialize() override;
  void Release() override;

  bool Create(const Rect &rect) override;
  bool CreateTransparent(const Rect &rect);
  void Destroy();

  void SetWindowTitle(const char *title);
  std::string GetWindowTitle();
  void SetBorderless(bool no_border = true);
  void SetMinMaxSize(int wMin, int hMin, int wMax, int hMax);
  void SetWindowRect(int x, int y, int width, int height);
  void GetClientRect(Rect *prect);
  void GetWindowRect(Rect *prect);
  void CenterWindow();
  bool IsChildWindow() override;
  bool IsWindowVisible() override;
  ::Window GetParentWindow();

  void Attach(HWND window);
  HWND Detach();

  void Show() override;
  void Hide();

  void Submit(IRenderTarget* pRT, const Rect* prect, int count) override;
  void Invalidate();

  void RECT2Rect(Rect& r, Rect* prect);
private:
  ui::Window &m_ui_window;
  HWND m_hWnd;
};

} // namespace ui

#endif