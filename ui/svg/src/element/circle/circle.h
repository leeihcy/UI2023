#ifndef _UI_SVG_SRC_ELEMENT_CIRCLE_CIRCLE_H_
#define _UI_SVG_SRC_ELEMENT_CIRCLE_CIRCLE_H_

#include "src/element/element.h"
namespace svg {

class Circle : public Element {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;

private:
  // 圆中心位置
  ui::Length m_cx;
  ui::Length m_cy;
  // 半径
  ui::Length m_r;
};

}
#endif