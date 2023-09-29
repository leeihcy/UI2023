#include "render_context.h"

float RenderContext::ResolveX(ui::Length& x) {
  if (x.unit == ui::Length::Unit::Percentage) {
    return view_box.width * x.value / 100;
  }
  return (float)x.value;
}
float RenderContext::ResolveY(ui::Length& y) {
  if (y.unit == ui::Length::Unit::Percentage) {
    return view_box.height * y.value / 100;
  }
  return (float)y.value;
}

float RenderContext::ResolveRX(ui::Length& rx) {
  float r = ResolveX(rx);
  return std::min((float)r, view_box.width / 2);
}
float RenderContext::ResolveRY(ui::Length& ry) {
  float r = ResolveY(ry);
  return std::min((float)r, view_box.height / 2);
}
