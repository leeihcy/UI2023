#ifndef _UI_SVG_INCLUDE_UISVGAPI_H_
#define _UI_SVG_INCLUDE_UISVGAPI_H_
#include "common.h"

namespace ui { 
struct IApplication;

extern "C"
void UISVGAPI SvgRegisterObjects(ui::IApplication* app);

}

#endif