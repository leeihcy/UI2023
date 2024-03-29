#include "rect.h"
#include "SkRRect.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Rect::ShapeRender(RenderContext &context, SkPaint& paint) {
  int x = context.ResolveX(m_x);
  int y = context.ResolveY(m_y);
  int width = context.ResolveX(m_width);
  int height = context.ResolveY(m_height);
  int rx = context.ResolveRX(m_rx);
  int ry = context.ResolveRY(m_ry);

  SkRect rect = SkRect::MakeXYWH(x, y, width, height);

  if (rx > 0 || ry > 0) {
    context.canvas->drawRRect(SkRRect::MakeRectXY(rect, rx, ry), paint);
  } else {
    context.canvas->drawRect(rect, paint);
  }
}

void Rect::SetAttribute(ui::SerializeParam &data) {
  Element::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Rect");
  s.AddLength("x", m_x);
  s.AddLength("y", m_y);
  s.AddLength("width", m_width);
  s.AddLength("height", m_height);
  s.AddLength("rx", m_rx);
  s.AddLength("ry", m_ry);
}
}