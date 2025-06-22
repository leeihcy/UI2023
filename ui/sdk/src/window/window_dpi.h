#ifndef _UI_SDK_SRC_WINDOW_WINDOW_DPI_H_
#define _UI_SDK_SRC_WINDOW_WINDOW_DPI_H_

namespace ui {
class Rect;

enum WindowScalePolicy { Raw, System, Force };

class WindowDPI {
public:
  WindowDPI();

  void ScaleRect(ui::Rect* rc);
  void RestoreRect(ui::Rect* rc);

  void SetSystemDpi(float dpi);
  
protected:
private:
  WindowScalePolicy m_policy;
  float m_scale;
};

} // namespace ui

#endif