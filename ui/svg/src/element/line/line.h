#ifndef _UI_SVG_SRC_ELEMENT_LINE_LILNE_H_
#define _UI_SVG_SRC_ELEMENT_LINE_LILNE_H_

#include "src/element/node.h"

class Line : public Node {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(const char* key, const char* value) override;

private:
  // 圆中心位置
  int m_x1 = 0;
  int m_x2 = 0;
  // 半径
  int m_y1 = 0;
  int m_y2 = 0;
};

#endif