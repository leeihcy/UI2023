#include "src/control/image/image.h"
#include "image_meta.h"
#include "include/common/signalslot/slot.h"
#include "include/interface/graphics.h"
#include "include/interface/ibundlesource.h"
#include "include/macro/msg.h"
#include "include/macro/xmldefine.h"
#include "include/uiapi.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/parser/datasource/bundle_source.h"


namespace ui {

Image::Image(IImage *p) : Control(p), m_pIImage(p) {}
Image::~Image() {
}

void Image::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_PAINT) {
    onPaint(static_cast<PaintMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_GETDESIREDSIZE) {
    onGetDesiredSize(&static_cast<GetDesiredSizeMessage *>(msg)->size);
    return;
  } else if (msg->message == UI_MSG_SERIALIZE) {
    auto *m = static_cast<SerializeMessage *>(msg);
    onSerialize(m->param);
    return;
  } else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto *m = static_cast<QueryInterfaceMessage *>(msg);
    if (m->uuid == ImageMeta::Get().UUID()) {
      *(m->pp) = m_pIImage;
      return;
    }
  } else if (msg->message == UI_MSG_FINALCONSTRUCT) {
    onFinalConstruct(static_cast<FinalConstructMessage *>(msg));
    return;
  }
  Control::onRouteMessage(msg);
}

void Image::onFinalConstruct(FinalConstructMessage *msg) {
  Control::onRouteMessage(msg);

  OBJSTYLE s = {0};
  s.default_transparent = 1;          // 默认透明
  s.default_reject_all_mouse_msg = 1; // 默认不接收鼠标消息

  OBJSTYLE sRemove = {0};
  sRemove.default_tabstop = 1; // 没有焦点
  ModifyObjectStyle(&s, &sRemove);
}

void Image::onPaint(IRenderTarget *r) {
  Rect rect;
  GetClientRectWithZeroOffset(&rect);

  if (m_bitmap) {
    DrawBitmapParam param;
    param.wDest = param.wSrc = m_bitmap->GetWidth();
    param.hDest = param.hSrc = m_bitmap->GetHeight();
    r->DrawBitmap(m_bitmap, &param);
  }

  if (m_fore_render) {
    m_fore_render->DrawState(r, &rect, 0);
  }
}

void Image::onSerialize(SerializeParam *pData) {
  Control::onSerialize(pData);

  AttributeSerializer s(pData, "Image");
  s.AddString(XML_SRC, Slot(&Image::loadSrc, this),
              Slot(&Image::saveSrc, this));
}

void Image::loadSrc(const char *src) {
  if (!src || !src[0]) {
    return;
  }
  m_src = src;

  m_bitmap = GetUIApplication()->CreateRenderBitmap(eGraphicsLibraryType::Skia,
                                                    eImageItemType::Image);
  if (!m_resource->LoadRenderBitmap(m_bitmap.get(), src)) {
    UI_LOG_WARN("[Image] load src failed: %s", src);
  }
}
const char *Image::saveSrc() { 
  return m_src.c_str(); 
}

void Image::onGetDesiredSize(Size *size) {
  size->width = size->height = 0;

  if (m_bitmap) {
    size->width = m_bitmap->GetWidth();
    size->height = m_bitmap->GetHeight();

    // TODO: dpi @ percent
  }

  REGION4 rcNonClient;
  m_pIImage->GetNonClientRegion(&rcNonClient);

  size->width += rcNonClient.left + rcNonClient.right;
  size->height += rcNonClient.top + rcNonClient.bottom;
}

} // namespace ui