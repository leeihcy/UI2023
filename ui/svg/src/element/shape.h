#ifndef _UI_SVG_SRC_ELEMENT_SHAPE_H_
#define _UI_SVG_SRC_ELEMENT_SHAPE_H_

#include "src/element/element.h"

namespace svg {
  
class Shape : public Element {
public:
  void PrepareRender(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;
};

}
#endif