#ifndef _UI_SVG_SRC_ELEMENT_POLYLINE_POLYLINE_H_
#define _UI_SVG_SRC_ELEMENT_POLYLINE_POLYLINE_H_

#include "src/element/shape.h"
#include "SkPath.h"

namespace svg {

class Polyline : public Shape {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;

  void AsPolygon() { m_is_polygon = true; }

protected:
  void load_points(const char* text);

private:
  SkPath m_path;
  bool m_is_polygon = false;
};

}
#endif