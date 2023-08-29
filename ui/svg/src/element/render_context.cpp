#include "render_context.h"

int RenderContext::ResolveX(ui::Length& x) {
  if (x.unit == ui::Length::Unit::Percentage) {
    return view_box.width * x.value / 100;
  }
  return x.value;
}
int RenderContext::ResolveY(ui::Length& y) {
  if (y.unit == ui::Length::Unit::Percentage) {
    return view_box.height * y.value / 100;
  }
  return y.value;
}

int RenderContext::ResolveRX(ui::Length& rx) {
  int r = ResolveX(rx);
  return std::min(r, view_box.width / 2);
}
int RenderContext::ResolveRY(ui::Length& ry) {
  int r = ResolveY(ry);
  return std::min(r, view_box.height / 2);
}
