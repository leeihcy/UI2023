#include "line.h"

void Line::Render(RenderContext &context) {
  context.canvas->drawLine(SkPoint::Make(m_x1, m_y1), SkPoint::Make(m_x2, m_y2),
                           context.paint);
}

void Line::SetAttribute(const char *key, const char *value) {
  if (strcmp(key, "x1") == 0) {
    m_x1 = atoi(value);
  } else if (strcmp(key, "x2") == 0) {
    m_x2 = atoi(value);
  } else if (strcmp(key, "y1") == 0) {
    m_y1 = atoi(value);
  } else if (strcmp(key, "y2") == 0) {
    m_y2 = atoi(value);
  }
}