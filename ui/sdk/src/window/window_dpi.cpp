#include "window_dpi.h"
#include "sdk/include/util/rect.h"

namespace ui {

WindowDPI::WindowDPI() {
  m_policy = WindowScalePolicy::System;
  m_scale = 1.0f;
}

void WindowDPI::ScaleRect(ui::Rect *rc) {
  if (m_policy == WindowScalePolicy::Raw) {
    return;
  }
  if (!rc) {
    return;
  }
  rc->left *= m_scale;
  rc->top *= m_scale;
  rc->right *= m_scale;
  rc->bottom *= m_scale;
}
void WindowDPI::RestoreRect(ui::Rect *rc) {
  if (m_policy == WindowScalePolicy::Raw) {
    return;
  }
  if (!rc) {
    return;
  }

  rc->left /= m_scale;
  rc->top /= m_scale;
  rc->right /= m_scale;
  rc->bottom /= m_scale;
}

void WindowDPI::SetSystemDpi(float dpi) { m_scale = dpi; }

} // namespace ui