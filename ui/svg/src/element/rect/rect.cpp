#include "rect.h"
#include "SkRRect.h"

void Rect::Render(RenderContext& context) {
  SkRect rect = SkRect::MakeXYWH(m_x, m_y, m_width, m_height);

  SkPaint paint;
  paint.setColor(SkColorSetARGB(255, 255, 0, 0));

  if (m_rx > 0 || m_ry > 0) {
    context.canvas->drawRRect(SkRRect::MakeRectXY(rect, m_rx, m_ry), context.paint);
  } else {
    context.canvas->drawRect(rect, paint);
  }
}

void Rect::SetAttribute(const char *key, const char *value) {
  if (strcmp(key, "x") == 0) {
    m_x = atoi(value);
  } else if (strcmp(key, "y") == 0) {
    m_y = atoi(value);
  } else if (strcmp(key, "width") == 0) {
    m_width = atoi(value);
  } else if (strcmp(key, "height") == 0) {
    m_height = atoi(value);
  } else if (strcmp(key, "rx") == 0) {
    m_rx = atoi(value);
  } else if (strcmp(key, "ry") == 0) {
    m_ry = atoi(value);
  }
}