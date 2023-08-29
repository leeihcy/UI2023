#include "line.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Line::Render(RenderContext &context) {
  int x1 = context.ResolveX(m_x1);
  int y1 = context.ResolveY(m_y1);
  int x2 = context.ResolveX(m_x2);
  int y2 = context.ResolveY(m_y2);

  context.canvas->drawLine(SkPoint::Make(x1, y1), SkPoint::Make(x2, y2),
                           context.paint);
}

void Line::SetAttribute(ui::SerializeParam &data) {
  Shape::SetAttribute(data);
  
  ui::AttributeSerializerWrap s(&data, "Line");
  s.AddLength("x1", m_x1);
  s.AddLength("x2", m_x2);
  s.AddLength("y1", m_y1);
  s.AddLength("y2", m_y2);
}

}