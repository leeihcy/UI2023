#include "include/interface/itextrenderbase.h"
#include "include/inc.h"
#include "include/interface/iuiapplication.h"
#include "include/interface/renderlibrary.h"
#include "sdk/include/util/struct.h"
#include "src/object/object.h"
#include "src/render/render_meta.h"
#include "src/render/textrender/textrender.h"
#include <cstddef>
#include <memory>

namespace ui {

UI_IMPLEMENT_INTERFACE(TextRenderBase, Message)
// UI_IMPLEMENT_INTERFACE(ThemeTextRenderBase, TextRenderBase)
UI_IMPLEMENT_INTERFACE(SimpleTextRender, TextRenderBase)
// UI_IMPLEMENT_INTERFACE(ContrastColorTextRender, TextRenderBase)
// UI_IMPLEMENT_INTERFACE(ContrastColorListTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(ColorListTextRender, TextRenderBase)
// UI_IMPLEMENT_INTERFACE(FontColorListTextRender, TextRenderBase)

ITextRenderBase::ITextRenderBase() : IMessage(CREATE_IMPL_FALSE) {
  m_pImpl = new TextRenderBase(this);
}

void ITextRenderBase::SetObject(IObject *pObject) {
  Object *pObj = nullptr;
  if (pObject)
    pObj = pObject->GetImpl();

  __pImpl->SetObject(pObj);
}
IObject *ITextRenderBase::GetObject() {
  Object *pObj = __pImpl->GetObject();
  if (pObj)
    return pObj->GetIObject();
  return nullptr;
}
void ITextRenderBase::SetTextAlignment(int nDrawFlag) {
  return __pImpl->SetTextAlignment(nDrawFlag);
}
int ITextRenderBase::GetTextAlignment() { return __pImpl->GetTextAlignment(); }
// void ITextRenderBase::SetTextEffect(TEXT_EFFECT e) {
//   __pImpl->SetTextEffect(e);
// }
// TEXT_EFFECT ITextRenderBase::GetTextEffect() {
//   return __pImpl->GetTextEffect();
// }
// void ITextRenderBase::SetDrawTextParam(long w, long l) {
//   __pImpl->SetDrawTextParam(w, l);
// }
void ITextRenderBase::SetType(const TEXTRENDER_TYPE &nType) {
  __pImpl->SetTextRenderType(nType);
}
TEXTRENDER_TYPE ITextRenderBase::GetType() {
  return __pImpl->GetTextRenderType();
}

IRenderFont *ITextRenderBase::GetRenderFont() {
    assert(false);
#if 0 // 废弃，使用RouteMessage代替。
  return (IRenderFont *)(static_cast<IMessage *>(this)->SendMessage(
      UI_MSG_GETRENDERFONT));
#endif
    return nullptr;
}

void ITextRenderBase::Serialize(SerializeParam *pData) {
  SerializeMessage msg;
  msg.param = pData;
  static_cast<IMessage*>(this)->RouteMessage(&msg);
}

// std::shared_ptr<IRenderFont> ITextRenderBase::_LoadFont(const char *szFontId) {
//   return __pImpl->_LoadFont(szFontId);
// }
// const char *ITextRenderBase::_SaveFont(IRenderFont *&pRenderFont) {
//   return __pImpl->_SaveFont(pRenderFont);
// }
// std::shared_ptr<IRenderFont> ITextRenderBase::_LoadDefalutFont() {
//   return __pImpl->_LoadDefalutFont();
// }
// void ITextRenderBase::_LoadColor(const char *szColorId, Color *&pColor) {
//   __pImpl->_LoadColor(szColorId, pColor);
// }
// const char *ITextRenderBase::_SaveColor(Color *&pColor) {
//   return __pImpl->_SaveColor(pColor);
// }
Size ITextRenderBase::GetDesiredSize(const char *szText, unsigned int nLimitWidth) {
  return __pImpl->GetDesiredSize(szText, nLimitWidth);
}
void ITextRenderBase::DrawState(IRenderTarget *pRenderTarget, const Rect *prc,
                                int nState, const char *szText,
                                int nDrawTextFlag) {
  TextRenderDrawStateMessage msg;
  msg.draw_state.ds_renderbase.pRenderTarget = pRenderTarget;
  if (prc) {
    memcpy(&msg.draw_state.ds_renderbase.rc, prc, sizeof(Rect));
  }
  msg.draw_state.ds_renderbase.nState = nState;

  msg.draw_state.szText = szText;
  msg.draw_state.nDrawTextFlag = nDrawTextFlag;
  static_cast<IMessage*>(this)->RouteMessage(&msg);
}

void ITextRenderBase::Init() {
    ui::Msg msg = { .message = UI_MSG_INITIALIZE };
    RouteMessage(&msg);
}
void ITextRenderBase::CheckSkinTextureChanged() {
  return __pImpl->CheckSkinTextureChanged();
}
void IColorListTextRender::SetCount(int nCount) { __pImpl->SetCount(nCount); }
void IColorListTextRender::SetColor(int nIndex, Color col) {
  __pImpl->SetColor(nIndex, col);
}

//
// const char*  IThemeTextRenderBase::GetThemeName()
// {
//     const char* pszName = nullptr;
//     UISendMessage(static_cast<IMessage*>(this),
//         UI_WM_RENDERBASE_GETTHEMENAME, (long)&pszName, 0);
//     return pszName;
// }
//
// HTHEME  IThemeTextRenderBase::GetTHEME()
// {
//     return __pImpl->GetTHEME();
// }
// void  IThemeTextRenderBase::Serialize(AttributeSerializerWrap* ps)
// {
// 	__pImpl->Serialize(ps);
// }

//////////////////////////////////////////////////////////////////////////

// void ISimpleTextRender::SetColor(Color *pColText) {
//   return __pImpl->SetColor(pColText);
// }
// void ISimpleTextRender::SetRenderFont(IRenderFont *p) {
//   __pImpl->SetRenderFont(p);
// }
// bool ISimpleTextRender::GetColor(Color &color) {
//   return __pImpl->GetColor(color);
// }

//////////////////////////////////////////////////////////////////////////
#if 0
//////////////////////////////////////////////////////////////////////////

void IFontColorListTextRender::SetCount(int nCount) {
  return __pImpl->SetCount(nCount);
}
void IFontColorListTextRender::SetColor(int nIndex, unsigned int color) {
  return __pImpl->SetColor(nIndex, color);
}
void IFontColorListTextRender::SetFont(int nIndex, std::shared_ptr<IRenderFont> p) {
  return __pImpl->SetFont(nIndex, p);
}
#endif
} // namespace ui