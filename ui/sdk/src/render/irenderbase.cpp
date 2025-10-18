#include "include/interface/irenderbase.h"
#include "include/inc.h"
#include "include/interface/iuiapplication.h"
#include "sdk/include/util/struct.h"
#include "src/object/object.h"
#include "src/render/render_meta.h"
#include "src/render/renderbase.h"
#include "src/resource/colorres.h"
#include "src/resource/imageres.h"

namespace ui {
UI_IMPLEMENT_INTERFACE(RenderBase, Message)
// UI_IMPLEMENT_INTERFACE(NullRender, RenderBase)

void IRenderBase::SetType(RENDER_TYPE nType) { __pImpl->SetRenderType(nType); }
RENDER_TYPE IRenderBase::GetType() { return __pImpl->GetRenderType(); }
IColorRes *IRenderBase::GetSkinColorRes() {
  ColorRes *p = __pImpl->GetSkinColorRes();
  if (p)
    return &p->GetIColorRes();

  return nullptr;
}
IImageRes *IRenderBase::GetSkinImageRes() {
  ImageRes *p = __pImpl->GetSkinImageRes();
  if (p)
    return &p->GetIImageRes();

  return nullptr;
}

void IRenderBase::Serialize(SerializeParam *pData) {
  SerializeMessage msg;
  msg.param = pData;
  static_cast<IMessage *>(this)->RouteMessage(&msg);
}

void IRenderBase::DrawState(IRenderTarget *p, const Rect *prc, int nState) {
  RenderBaseDrawStateMessage msg;
  msg.draw_state.pRenderTarget = p;
  if (prc) {
    msg.draw_state.rc.CopyFrom(*prc);
  }
  msg.draw_state.nState = nState;

  static_cast<IMessage *>(this)->RouteMessage(&msg);
}

Size IRenderBase::GetDesiredSize() {
  GetDesiredSizeMessage msg;
  RouteMessage(&msg);
  return msg.size;
}
void IRenderBase::Init() {
    RouteMessage(UI_MSG_INITIALIZE);
}
} // namespace ui