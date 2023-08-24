#include "sdk/include/interface/iuiapplication.h"

#include "include/uisvgapi.h"
#include "src/element/rect/rect_meta.h"
#include "src/svg/svg_meta.h"
#include "src/svg/svg_layout.h"

namespace ui { namespace svg {

bool CreateSvgLayout(IObject *pObject, ILayout **ppLayout) {
  SvgLayout* layout = new SvgLayout(pObject);
  *ppLayout = static_cast<ui::ILayout*>(layout);
  return true;
}

extern "C"
UISVGAPI void RegisterObjects(ui::IApplication* app) {
  app->RegisterUIObject(&SvgMeta::Get());
  app->RegisterUIObject(&RectMeta::Get());

  app->RegisterLayout(L"svg", CreateSvgLayout);
}


}}