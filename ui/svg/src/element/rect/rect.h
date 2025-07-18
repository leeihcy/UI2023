#ifndef _UI_SVG_SRC_ELEMENT_RECT_RECT_H_
#define _UI_SVG_SRC_ELEMENT_RECT_RECT_H_

#include "src/element/shape/shape.h"
#include "sdk/include/util/rect.h"

namespace svg {
  
class Rect : public Shape {
public:
  void ShapeRender(RenderContext& context, SkPaint& paint) override;
  void SetAttribute(ui::SerializeParam& data) override;

private:
  // Value	<length> | <percentage>
  // Default value	0
  // Animatable	Yes
  ui::Length m_x;
  ui::Length m_y;
  // Value: auto | <length> | <percentage>
  // Default value: auto (treated as 0)
  ui::Length m_width;
  ui::Length m_height;
  // 	<length> | <percentage> | auto
  ui::Length m_rx;
  ui::Length m_ry;
};

}
#endif