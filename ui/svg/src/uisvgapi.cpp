#include "sdk/include/interface/iuiapplication.h"

#include "include/uisvgapi.h"
// #include "src/element/rect/rect_meta.h"
#include "src/svg/svg_layout.h"
#include "src/svg/svg_meta.h"

namespace ui {
namespace svg {

// bool CreateSvgLayout(IObject *pObject, ILayout **ppLayout) {
//   SvgLayout* layout = new SvgLayout(pObject);
//   *ppLayout = static_cast<ui::ILayout*>(layout);
//   return true;
// }

PARSE_CONTROL_RETURN UIParseSvgTag(IUIElement *pElem, IResource *pSkinRes,
                                   IObject *pParent, IObject **ppOut) {
  return ParseControl_LoadDescendants;          
}

extern "C" UISVGAPI void RegisterObjects(ui::IApplication *app) {
  app->RegisterUIObject(&SvgMeta::Get());
  app->RegisterControlTagParseFunc("svg", UIParseSvgTag);

  // app->RegisterLayout(L"svg", CreateSvgLayout);
}

} // namespace svg
} // namespace ui