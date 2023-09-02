#include "include/interface/isvg.h"
#include "src/ui/svg_control.h"
#include "src/ui/svg_meta.h"

namespace ui {

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(Svg, Panel)

bool ISvg::Load(const char* stream) {
  return m_pImpl->Load(stream);
}

} // namespace ui