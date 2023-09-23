#include "shape.h"

namespace svg {

void Shape::Render(RenderContext &ctx) {
  if (ctx.fill) {
    this->ShapeRender(ctx, ctx.fill_paint);
  }
  if (ctx.stroke) {
    this->ShapeRender(ctx, ctx.stroke_paint);
  }
}

} // namespace svg
