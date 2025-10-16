#ifndef _UI_ICOLORRENDER_H_
#define _UI_ICOLORRENDER_H_
#include "irenderbase.h"
#include "include/util/color.h"

namespace ui {

class ColorRender;
struct UIAPI IColorRender : public IRenderBase {
  void SetBackColor(Color color);
  void SetBorderColor(Color color);
  void SetBackColor(Color color, int index);
  void SetBorderColor(Color color, int index);
  
  void SetBorder(int width);

  UI_DECLARE_INTERFACE(ColorRender);
};

} // namespace ui

#endif // _UI_ICOLORRENDER_H_