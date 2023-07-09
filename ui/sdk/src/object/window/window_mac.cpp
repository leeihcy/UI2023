#include "window_mac.h"
#include <string.h>

namespace ui {

void WindowPlatformMac::Initialize(ui::Window *ui_window) {
  m_ui_window = ui_window;
}

void WindowPlatformMac::Release() {
  Destroy();
}

void WindowPlatformMac::Create(const Rect &rect) {
}
 
void WindowPlatformMac::Destroy() {
}
} // namespace ui