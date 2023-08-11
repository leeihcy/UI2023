#ifndef _UI_SVG_INCLUDE_INTERFACE_IRECT_H_
#define _UI_SVG_INCLUDE_INTERFACE_IRECT_H_

#include "../common.h"
#include "sdk/include/interface/iobject.h"

namespace ui { namespace svg {
class Rect;

struct UISVGAPI IRect : public ui::IObject {

  UI_DECLARE_INTERFACE_ACROSSMODULE(Rect);
public:
  UI_DEFINE_CLASS_GUID("473cce67-2db5-4a58-8e0e-0a9fc09012fc");
};

}}

#endif