#include "render_context.h"

int RenderContext::ResolveX(ui::Length& x) {
  if (x.unit == ui::Length::Unit::Percentage) {
    return svg_size.width * x.value / 100;
  }
  return x.value;
}
int RenderContext::ResolveY(ui::Length& y) {
  if (y.unit == ui::Length::Unit::Percentage) {
    return svg_size.height * y.value / 100;
  }
  return y.value;
}
