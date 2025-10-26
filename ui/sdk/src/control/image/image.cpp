#include "src/control/image/image.h"
#include "include/common/signalslot/slot.h"
#include "include/macro/msg.h"
#include "image_meta.h"
#include "include/uiapi.h"
#include "src/attribute/attribute.h"

namespace ui {

Image::Image(IImage *p) : Control(p), m_pIImage(p) {
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
      *(m->pp) = m_pIControl;
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
  s.AddString(XML_SRC, Slot(&Image::loadSrc, this), Slot(&Image::saveSrc, this));
}

void Image::loadSrc(const char *src) {
  // UICreateRenderBitmap(IApplication *pUIApp, eGraphicsLibraryType eGraphicsRenderType, IMAGE_ITEM_TYPE eType)
}
const char *Image::saveSrc() {
  return nullptr;
}

void Image::onGetDesiredSize(Size *size) {
  size->width = size->height = 0;

#if 0 // TODO:
  ITextRenderBase *pTextRender = m_pIImage->GetTextRenderDefault();
  if (pTextRender) {
    LPCTSTR szText = m_strText.c_str();
    if (0 == szText[0]) {
      // TBD: 内容为空时，至少保证高度
      szText = TEXT(" ");
      *pSize = pTextRender->GetDesiredSize(szText, m_pIImage->GetMaxWidth());
      pSize->cx = 0;
    } else {
      *pSize = pTextRender->GetDesiredSize(szText, m_pIImage->GetMaxWidth());
    }
  }
#endif
  REGION4 rcNonClient;
  m_pIImage->GetNonClientRegion(&rcNonClient);

  size->width += rcNonClient.left + rcNonClient.right;
  size->height += rcNonClient.top + rcNonClient.bottom;
}

} // namespace ui