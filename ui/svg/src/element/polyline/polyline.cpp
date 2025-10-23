#include "polyline.h"
#include "sdk/include/util/util.h"
#include "sdk/include/interface/imapattr.h"

namespace svg {

void Polyline::ShapeRender(RenderContext &context, SkPaint& paint) {
    //fPath.setFillType(fillType);
    context.canvas->drawPath(m_path, paint);
}

void Polyline::SetAttribute(ui::SerializeParam &data) {
  Shape::SetAttribute(data);

  const char *text = data.attribute_map->GetAttr("points", true);
  load_points(text);
}

void Polyline::load_points(const char *text) {
  if (!text) {
    return;
  }
  ui::util::ISplitStringEnum *penum = nullptr;
  ui::util::SplitString(text, ' ', &penum);
  if (!penum) {
    return;
  }
  std::vector<SkPoint> points;
  for (int i = 0; i < penum->GetCount(); i+=2) {
    points.push_back(
      SkPoint::Make(
      atoi(penum->GetText(i)),
      atoi(penum->GetText(i+1))
      )
    );
  }
  penum->Release();

  m_path = SkPath::Polygon(&(*points.begin()), points.size(), m_is_polygon);
}

} // namespace svg