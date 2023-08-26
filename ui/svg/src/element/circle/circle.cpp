#include "circle.h"
#include "SkRRect.h"

void Circle::Render(RenderContext& context) {
    SkPoint pos = SkPoint::Make(m_cx, m_cy);
    SkScalar r = m_r;
    
    if (r > 0) {
        context.canvas->drawCircle(pos.x(), pos.y(), r, context.paint);
    }
}

void Circle::SetAttribute(const char *key, const char *value) {
  if (strcmp(key, "cx") == 0) {
    m_cx = atoi(value);
  } else if (strcmp(key, "cy") == 0) {
    m_cy = atoi(value);
  } else if (strcmp(key, "r") == 0) {
    m_r = atoi(value);
  }
}