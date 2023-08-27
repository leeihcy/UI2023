#ifndef _UI_SVG_SRC_ELEMENT_LINE_LILNE_H_
#define _UI_SVG_SRC_ELEMENT_LINE_LILNE_H_

#include "src/element/element.h"

class Line : public Element {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;

private:
  // 圆中心位置
  ui::Length m_x1;
  ui::Length m_x2;
  // 半径
  ui::Length m_y1;
  ui::Length m_y2;
};

#endif