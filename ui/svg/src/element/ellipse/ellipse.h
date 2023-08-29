#ifndef _UI_SVG_SRC_ELEMENT_ELLIPSE_ELLIPSE_H_
#define _UI_SVG_SRC_ELEMENT_ELLIPSE_ELLIPSE_H_

#include "src/element/shape.h"

namespace svg {

class Ellipse : public Shape {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;

private:
  // 圆中心位置
  ui::Length m_cx;
  ui::Length m_cy;
  // 半径
  ui::Length m_rx;
  ui::Length m_ry;
};

}
#endif