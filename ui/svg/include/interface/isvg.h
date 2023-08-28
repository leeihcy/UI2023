#ifndef _UI_SVG_INCLUDE_INTERFACE_ISVG_H_
#define _UI_SVG_INCLUDE_INTERFACE_ISVG_H_

#include "svg/include/common.h"
#include "sdk/include/common/ptr/unique_ptr.h"
#include "sdk/include/interface/ipanel.h"

namespace ui {
class Svg;

struct UISVGAPI ISvg : public ui::IPanel {
  bool Load(const char* stream);

  UI_DECLARE_INTERFACE_ACROSSMODULE(Svg)
};
UI_DEFINE_PTR(Svg);

} // namespace ui

#endif