#ifndef _UI_SVG_SRC_ELEMENT_RECT_RECT_H_
#define _UI_SVG_SRC_ELEMENT_RECT_RECT_H_

#include "src/element/node.h"

class Rect : public Node {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(const char* key, const char* value) override;

private:
  int m_x = 0;
  int m_y = 0;
  int m_width = 0;
  int m_height = 0;
  int m_rx = 0;
  int m_ry = 0;
};

#endif