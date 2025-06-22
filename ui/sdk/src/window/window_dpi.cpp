#include "window_dpi.h"

namespace ui {

WindowDPI::WindowDPI() {
  m_policy = WindowScalePolicy::System;
  m_scale = 1.0f;
}

void WindowDPI::ScaleRect(ui::Rect *rc) {
  if (m_policy == WindowScalePolicy::Raw) {
    return;
  }
}
void WindowDPI::RestoreRect(ui::Rect *rc) {
  if (m_policy == WindowScalePolicy::Raw) {
    return;
  }
}

void WindowDPI::SetSystemDpi(float dpi) { m_scale = dpi; }

} // namespace ui