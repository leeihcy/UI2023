#ifndef _UI_SVG_INCLUDE_INTERFACE_IRECT_H_
#define _UI_SVG_INCLUDE_INTERFACE_IRECT_H_
#include "svg/include/common.h"
#include "sdk/include/common/ptr/unique_ptr.h"
#include "sdk/include/interface/iobject.h"

namespace ui {
namespace svg {
class Rect;

struct UISVGAPI IRect : public ui::IObject {
  UI_DECLARE_INTERFACE_ACROSSMODULE(Rect)
};
UI_DEFINE_PTR(Rect);

} 
}  

#endif