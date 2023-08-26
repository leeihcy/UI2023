#ifndef _UI_SVG_SRC_ELEMENT_ELLIPSE_ELLIPSE_H_
#define _UI_SVG_SRC_ELEMENT_ELLIPSE_ELLIPSE_H_

#include "src/element/node.h"

class Ellipse : public Node {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(const char* key, const char* value) override;

private:
  // 圆中心位置
  int m_cx = 0;
  int m_cy = 0;
  // 半径
  int m_rx = 0;
  int m_ry = 0;
};

#endif