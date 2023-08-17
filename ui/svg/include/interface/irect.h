#ifndef _UI_SVG_INCLUDE_INTERFACE_IRECT_H_
#define _UI_SVG_INCLUDE_INTERFACE_IRECT_H_
#include "svg/include/common.h"
#include "sdk/include/common/ptr/unique_ptr.h"
#include "sdk/include/interface/iobject.h"

namespace ui {
namespace svg {
class Rect;

struct UISVGAPI IRect : public ui::IObject {

  static Uuid UUID() { return Uuid("e9548da8-38ca-11ee-ac24-f45c89b0174f"); }

  UI_DECLARE_INTERFACE_ACROSSMODULE(Rect)
};
using RectPtr = ui::unique_ptr<IRect>;

} 
}  

#endif