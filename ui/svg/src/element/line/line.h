#ifndef _UI_SVG_SRC_ELEMENT_LINE_LILNE_H_
#define _UI_SVG_SRC_ELEMENT_LINE_LILNE_H_

#include "src/element/shape/shape.h"

namespace svg {

class Line : public Shape {
public:
  void ShapeRender(RenderContext& context, SkPaint& paint) override;
  void SetAttribute(ui::SerializeParam& data) override;

private:
  // 圆中心位置
  ui::Length m_x1;
  ui::Length m_x2;
  // 半径
  ui::Length m_y1;
  ui::Length m_y2;
};

}
#endif