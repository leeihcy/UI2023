#ifndef _UI_SDK_SRC_WINDOW_WINDOW_DPI_H_
#define _UI_SDK_SRC_WINDOW_WINDOW_DPI_H_

namespace ui {
struct Rect;
struct Size;

enum WindowScalePolicy { Raw, System, Force };

enum class CoordUnit {
 dip, // 逻辑坐标
 px,  // 物理坐标
};

class WindowDPI {
public:
  WindowDPI();
  float GetScaleFactor();
  
  void ScaleRect(ui::Rect* rc);
  void RestoreRect(ui::Rect* rc);
  void ScaleSize(ui::Size* size);

  void SetSystemDpi(float dpi);
  
protected:
private:
  WindowScalePolicy m_policy;
  float m_scale;
};

} // namespace ui

#endif