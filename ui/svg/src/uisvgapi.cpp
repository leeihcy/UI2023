#include "include/uisvgapi.h"
#include "src/element/rect/rect_desc.h"

namespace ui { namespace svg {


extern "C"
void UISVGAPI RegisterObjects(ui::IApplication* app) {
  app->RegisterUIObject(RectDescription::Get());
}


}}