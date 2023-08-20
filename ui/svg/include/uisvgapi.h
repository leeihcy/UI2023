#ifndef _UI_SVG_INCLUDE_UISVGAPI_H_
#define _UI_SVG_INCLUDE_UISVGAPI_H_
#include "common.h"

namespace ui { 
struct IApplication;

namespace svg {

extern "C"
void UISVGAPI RegisterObjects(ui::IApplication* app);

}}

#endif