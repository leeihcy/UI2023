#include "sdk/include/interface/iuiapplication.h"

#include "include/uisvgapi.h"
// #include "src/element/rect/rect_meta.h"
#include "src/ui/svg_layout.h"
#include "src/ui/svg_meta.h"

namespace ui {

// bool CreateSvgLayout(IObject *pObject, ILayout **ppLayout) {
//   SvgLayout* layout = new SvgLayout(pObject);
//   *ppLayout = static_cast<ui::ILayout*>(layout);
//   return true;
// }

PARSE_CONTROL_RETURN UIParseSvgTag(IUIElement *pElem, IResourceBundle *resource_bundle,
                                   IObject *pParent, IObject **ppOut) {
  return ParseControl_LoadDescendants;          
}

extern "C" UISVGAPI void SvgRegisterObjects(ui::IApplication *app) {
  app->RegisterUIObject(&SvgMeta::Get());
  app->RegisterControlTagParseFunc("svg", UIParseSvgTag);

  // app->RegisterLayout(L"svg", CreateSvgLayout);
}

} // namespace ui