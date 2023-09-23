#ifndef _UI_SVG_SRC_ELEMENT_SHAPE_H_
#define _UI_SVG_SRC_ELEMENT_SHAPE_H_

#include "src/element/element.h"

namespace svg {
  
class Shape : public Element {
public:
  void Render(RenderContext& context) override;

public:
  // shape子类要实现这个Render方法。
  virtual void ShapeRender(RenderContext &context, SkPaint& paint) = 0;
};

}
#endif