#include "rect.h"
#include "SkRRect.h"
#include "sdk/include/interface/iattribute.h"

void Rect::Render(RenderContext &context) {
  int x = context.ResolveX(m_x);
  int y = context.ResolveY(m_y);
  int width = context.ResolveX(m_width);
  int height = context.ResolveY(m_height);
  int rx = context.ResolveX(m_rx);
  int ry = context.ResolveY(m_ry);

  SkRect rect = SkRect::MakeXYWH(x, y, width, height);

  if (m_fill.value) {
    context.paint.setColor(m_fill.value);
  }
  
  if (rx > 0 || ry > 0) {
    context.canvas->drawRRect(SkRRect::MakeRectXY(rect, rx, ry), context.paint);
  } else {
    context.canvas->drawRect(rect, context.paint);
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