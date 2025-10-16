#include "include/inc.h"
#include "include/interface/icolorrender.h"
#include "src/render/colorrender/colorrender.h"
#include "src/render/render_meta.h"

namespace ui {
UI_IMPLEMENT_INTERFACE(ColorRender, RenderBase)

void IColorRender::SetBackColor(Color color) {
  return __pImpl->SetBackColor(color);
}
void IColorRender::SetBorderColor(Color col) {
  return __pImpl->SetBorderColor(col);
}
void IColorRender::SetBackColor(Color color, int index) {
  return __pImpl->SetBackColor(color, index);
}
void IColorRender::SetBorderColor(Color color, int index) {
  return __pImpl->SetBorderColor(color, index);
}
void IColorRender::SetBorder(int width) { __pImpl->SetBorder(width); }

} // namespace ui