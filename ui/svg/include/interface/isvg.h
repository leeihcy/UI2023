#ifndef _UI_SVG_INCLUDE_INTERFACE_IRECT_H_
#define _UI_SVG_INCLUDE_INTERFACE_IRECT_H_

#include "../common.h"
#include "sdk/include/interface/ipanel.h"

namespace ui {
namespace svg {
class Svg;



struct UISVGAPI ISvg : public ui::IPanel {

  static Uuid UUID() { return Uuid("3855b19a-39b6-11ee-9f56-f45c89b0174f"); }

  UI_DECLARE_INTERFACE_ACROSSMODULE(Svg)
};

} // namespace svg
} // namespace ui

#endif