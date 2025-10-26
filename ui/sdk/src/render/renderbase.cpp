#include "renderbase.h"
#include "include/interface/iuires.h"
#include "include/interface/renderlibrary.h"
#include "include/macro/msg.h"
#include "include/macro/xmldefine.h"
#include "src/attribute/attribute.h"
#include "src/object/message.h"
#include "src/object/object.h"
#include "src/resource/res_bundle.h"

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

ResourceBundle *RenderBase::GetResource() {
  return m_resouce;
}
ColorRes *RenderBase::GetSkinColorRes() {
  ResourceBundle *resource_bundle = GetResource();
  if (!resource_bundle)
    return nullptr;

  return &resource_bundle->GetColorRes();
}

ImageRes *RenderBase::GetSkinImageRes() {
  ResourceBundle *resource_bundle = GetResource();
  if (!resource_bundle)
    return nullptr;

  return &resource_bundle->GetImageRes();
}

