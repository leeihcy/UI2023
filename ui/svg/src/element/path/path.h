#ifndef _UI_SVG_SRC_ELEMENT_PATH_PATH_H_
#define _UI_SVG_SRC_ELEMENT_PATH_PATH_H_

#include "src/element/shape/shape.h"
#include "SkPath.h"

namespace svg {

class Path : public Shape {
public:
  void ShapeRender(RenderContext& context, SkPaint& paint) override;
  void SetAttribute(ui::SerializeParam& data) override;

private:
  SkPath m_path;
};

}
#endif