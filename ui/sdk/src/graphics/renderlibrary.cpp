#include "include/inc.h"

#include "include/interface/iwindow.h"
#include "include/interface/renderlibrary.h"
#include "include/interface/iuiapplication.h"
#include "src/application/uiapplication.h"
#include "src/application/config/config.h"
#include "include/macro/xmldefine.h"
#include "src/graphics/record/record_render_target.h"
#include "src/graphics/skia/skia_render.h"
#include "src/graphics/skia/skia_bitmap.h"
#include "src/graphics/skia/skia_render.h"
#include <memory>

namespace ui {

std::shared_ptr<IRenderBitmap> RenderBitmapFactory::CreateInstance(
    IApplication *pUIApp, GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType,
    IMAGE_ITEM_TYPE eType) {

  switch (eGraphicsRenderType) {
  case GRAPHICS_RENDER_LIBRARY_TYPE_SKIA: {
    if (eType == IMAGE_ITEM_TYPE_ICON) {
      // GdiplusIconRenderBitmap::CreateInstance(ppOut);
    } else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST) {
      // GdiplusImageListRenderBitmap::CreateInstance(ppOut);
    } else {
      return SkiaRenderBitmap::CreateInstance();
    }
  } break;

  default:
    UIASSERT(0);
    break;
  }
  return std::shared_ptr<IRenderBitmap>();
}

// IRenderTarget* 没有引用计数机制
IRenderTarget *UICreateRenderTarget(IApplication *pUIApp,
                                    GRAPHICS_RENDER_LIBRARY_TYPE eType,
                                    bool bNeedAlphaChannel) {
  IRenderTarget *pRenderTarget = nullptr;

  switch (eType) {
  case GRAPHICS_RENDER_LIBRARY_TYPE_SKIA: {
    if (Config::GetInstance().enable_render_thread) {
      pRenderTarget = new RecordRenderTarget();
    } else {
      pRenderTarget = new SkiaRenderTarget();
    }
  } break;

  default:
    UIASSERT(0);
    break;
  }
  return pRenderTarget;
}

std::shared_ptr<IRenderBitmap>
UICreateRenderBitmap(IApplication *pUIApp,
                     GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType,
                     IMAGE_ITEM_TYPE eType) {
  return RenderBitmapFactory::CreateInstance(pUIApp, eGraphicsRenderType,
                                             eType);
}

} // namespace ui