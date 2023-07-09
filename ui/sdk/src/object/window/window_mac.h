#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_MAC_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_MAC_H_
#include "window.h"

#include <string>

namespace ui {
class WindowPlatformMac : public WindowPlatform {
public:
  void Initialize(ui::Window *) override;
  void Release() override;

  void Create(const Rect &rect) override;
  void Destroy();

  void Show() override{}

  void Submit(sk_sp<SkSurface> sksurface) override {}
private:
  ui::Window *m_ui_window = nullptr;
};

} // namespace ui

#endif