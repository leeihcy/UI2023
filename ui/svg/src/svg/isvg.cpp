#include "include/interface/isvg.h"
#include "src/svg/svg.h"
#include "src/svg/svg_meta.h"

namespace ui {
namespace svg {

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(Svg, Panel)

bool ISvg::Load(const char* stream) {
  return m_pImpl->Load(stream);
}

} // namespace svg
} // namespace ui