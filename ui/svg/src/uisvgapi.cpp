#include "sdk/include/interface/iuiapplication.h"

#include "include/uisvgapi.h"
#include "src/element/rect/rect_meta.h"
#include "src/svg/svg_meta.h"

namespace ui { namespace svg {


extern "C"
UISVGAPI void RegisterObjects(ui::IApplication* app) {
  app->RegisterUIObject(&SvgMeta::Get());
  app->RegisterUIObject(&RectMeta::Get());
}


}}