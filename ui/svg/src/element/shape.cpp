#include "shape.h"

namespace svg {

void Shape::PrepareRender(RenderContext& context) {
  if (m_fill.value) {
    context.paint.setColor(m_fill.value);
  }
  if (m_stroke.value) {
    context.paint.setStroke(true);
    context.paint.setColor(m_stroke.value);
    
    int stroke_width = context.ResolveX(m_stroke_width);
    context.paint.setStrokeWidth((SkScalar)stroke_width);
  }
}

void Shape::SetAttribute(ui::SerializeParam& data) {
  Element::SetAttribute(data);
}

}

