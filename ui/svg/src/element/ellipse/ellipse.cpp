#include "ellipse.h"
#include "SkRRect.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Ellipse::Render(RenderContext &context) {
  int cx = context.ResolveX(m_cx);
  int cy = context.ResolveY(m_cy);
  int rx = context.ResolveRX(m_rx);
  int ry = context.ResolveRY(m_ry);

  SkRect rect = (rx > 0 && ry > 0)
                    ? SkRect::MakeXYWH(cx - rx, cy - ry, rx * 2, ry * 2)
                    : SkRect::MakeEmpty();

  context.canvas->drawOval(rect, context.paint);
}

void Ellipse::SetAttribute(ui::SerializeParam &data) {
  Element::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Ellipse");
  s.AddLength("cx", m_cx);
  s.AddLength("cy", m_cy);
  s.AddLength("rx", m_rx);
  s.AddLength("ry", m_ry);
}

}