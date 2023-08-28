#include "circle.h"
#include "SkRRect.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Circle::Render(RenderContext &context) {
  int cx = context.ResolveX(m_cx);
  int cy = context.ResolveY(m_cy);
  int r = context.ResolveX(m_r);

  if (m_fill.value) {
    context.paint.setColor(m_fill.value);
  }
  
  SkPoint pos = SkPoint::Make(cx, cy);
  if (r > 0) {
    context.canvas->drawCircle(pos.x(), pos.y(), r, context.paint);
  }
}

void Circle::SetAttribute(ui::SerializeParam &data) {
  Element::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Circle");
  s.AddLength("cx", m_cx);
  s.AddLength("cy", m_cy);
  s.AddLength("r", m_r);
}

}