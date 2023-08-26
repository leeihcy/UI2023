#include "ellipse.h"
#include "SkRRect.h"

void Ellipse::Render(RenderContext& context) {
    SkRect rect = (m_rx > 0 && m_ry > 0)
        ? SkRect::MakeXYWH(m_cx - m_rx, m_cy - m_ry, m_rx * 2, m_ry * 2)
        : SkRect::MakeEmpty();
    
    context.canvas->drawOval(rect, context.paint);
}

void Ellipse::SetAttribute(const char *key, const char *value) {
  if (strcmp(key, "cx") == 0) {
    m_cx = atoi(value);
  } else if (strcmp(key, "cy") == 0) {
    m_cy = atoi(value);
  } else if (strcmp(key, "rx") == 0) {
    m_rx = atoi(value);
  } else if (strcmp(key, "ry") == 0) {
    m_ry = atoi(value);
  }
}