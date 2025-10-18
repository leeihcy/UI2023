#include "renderbase.h"
#include "include/inc.h"

#include "include/interface/iuires.h"
#include "include/interface/renderlibrary.h"
#include "include/macro/msg.h"
#include "include/macro/xmldefine.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/object/message.h"
#include "src/object/object.h"
#include "src/resource/res_bundle.h"

#if 0 // defined(OS_WIN)
#pragma comment(lib, "uxtheme.lib")
#endif
using namespace ui;

RenderBase::RenderBase(IRenderBase *p) : Message(p) {
  m_pIRenderBase = p;
  m_resouce = nullptr;
  m_nRenderType = RENDER_TYPE_NULL;
}

void RenderBase::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    Message::onRouteMessage(msg);
    m_resouce = static_cast<FinalConstructMessage *>(msg)->resource->GetImpl();
    return;
  } else if (msg->message == UI_MSG_SERIALIZE) {
    SerializeParam *param = static_cast<SerializeMessage *>(msg)->param;
    
    AttributeSerializer s(param, "RenderBase");
    s.AddRect(XML_RENDER_MARGIN, m_deflate_margin);
  }
}

Resource *RenderBase::GetResource() {
  return m_resouce;
}
ColorRes *RenderBase::GetSkinColorRes() {
  Resource *pSkinRes = GetResource();
  if (!pSkinRes)
    return nullptr;

  return &pSkinRes->GetColorRes();
}

ImageRes *RenderBase::GetSkinImageRes() {
  Resource *pSkinRes = GetResource();
  if (!pSkinRes)
    return nullptr;

  return &pSkinRes->GetImageRes();
}

std::shared_ptr<IRenderBitmap> RenderBase::_LoadBitmap(const char *id) {
  if (!id)
    return std::shared_ptr<IRenderBitmap>();

  // if (m_pObject) {
  //   Resource *pSkinRes = m_pObject->GetResource();
  //   if (!pSkinRes)
  //     return;

  //   GRAPHICS_RENDER_LIBRARY_TYPE eType = GRAPHICS_RENDER_LIBRARY_TYPE_SKIA;
  //   Window *pWnd = m_pObject->GetWindow();
  //   if (pWnd) {
  //     WindowRender *pRender = pWnd->GetWindowRender();
  //     if (pRender)
  //       eType = pRender->GetGraphicsRenderType();
  //   }

  //   pSkinRes->GetImageRes().GetBitmap(szBitmapId, eType, &pBitmapRef);
  // } else {
    ImageRes *pImageRes = GetSkinImageRes();
    if (!pImageRes)
      return std::shared_ptr<IRenderBitmap>();

    return pImageRes->GetBitmap(id, GRAPHICS_RENDER_LIBRARY_TYPE_SKIA);
  // }
}

const char *RenderBase::_GetBitmapId(IRenderBitmap *pBitmap) {
  if (!pBitmap)
    return nullptr;

  ImageRes *pImageRes = GetSkinImageRes();
  if (!pImageRes)
    return nullptr;

  return pImageRes->GetRenderBitmapId(pBitmap);
}
