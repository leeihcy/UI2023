#include "include/interface/itextrenderbase.h"
#include "include/inc.h"
#include "include/interface/iuiapplication.h"
#include "src/object/object.h"
#include "src/render/render_meta.h"
#include "src/render/textrender/textrender.h"

namespace ui {

UI_IMPLEMENT_INTERFACE(TextRenderBase, Message)
// UI_IMPLEMENT_INTERFACE(ThemeTextRenderBase, TextRenderBase)
UI_IMPLEMENT_INTERFACE(SimpleTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(ContrastColorTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(ContrastColorListTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(ColorListTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(FontColorListTextRender, TextRenderBase)

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
void ITextRenderBase::SetTextEffect(TEXT_EFFECT e) {
  __pImpl->SetTextEffect(e);
}
TEXT_EFFECT ITextRenderBase::GetTextEffect() {
  return __pImpl->GetTextEffect();
}
void ITextRenderBase::SetDrawTextParam(long w, long l) {
  __pImpl->SetDrawTextParam(w, l);
}
void ITextRenderBase::SetType(const TEXTRENDER_TYPE &nType) {
  __pImpl->SetTextRenderType(nType);
}
TEXTRENDER_TYPE ITextRenderBase::GetType() {
  return __pImpl->GetTextRenderType();
}

IRenderFont *ITextRenderBase::GetRenderFont() {
  return (IRenderFont *)(static_cast<IMessage *>(this)->SendMessage(
      UI_MSG_GETRENDERFONT));
}

void ITextRenderBase::Serialize(SerializeParam *pData) {
  SerializeMessage msg;
  msg.param = pData;
  static_cast<IMessage*>(this)->RouteMessage(&msg);
}

void ITextRenderBase::_LoadFont(const char *szFontId,
                                IRenderFont *&pRenderFont) {
  __pImpl->_LoadFont(szFontId, pRenderFont);
}
const char *ITextRenderBase::_SaveFont(IRenderFont *&pRenderFont) {
  return __pImpl->_SaveFont(pRenderFont);
}
void ITextRenderBase::_LoadDefalutFont(IRenderFont **ppRenderFont) {
  __pImpl->_LoadDefalutFont(ppRenderFont);
}
void ITextRenderBase::_LoadColor(const char *szColorId, Color *&pColor) {
  __pImpl->_LoadColor(szColorId, pColor);
}
const char *ITextRenderBase::_SaveColor(Color *&pColor) {
  return __pImpl->_SaveColor(pColor);
}

Size ITextRenderBase::GetDesiredSize(const char *szText, int nLimitWidth) {
  //     Size s = {0,0};
  //
  //     GETDESIREDSIZEINFO info;
  //     info.nLimitWidth = nLimitWidth;
  //     info.szText = szText;
  //     UISendMessage(static_cast<IMessage*>(this), UI_WM_GETDESIREDSIZE,
  //     (long)&s, (long)&info);
  //
  //     return s;

  return __pImpl->GetDesiredSize(szText, nLimitWidth);
}
void ITextRenderBase::DrawState(IRenderTarget *pRenderTarget, const Rect *prc,
                                int nState, const char *szText,
                                int nDrawTextFlag) {
  TEXTRENDERBASE_DRAWSTATE drawstate = {0};
  drawstate.ds_renderbase.pRenderTarget = pRenderTarget;
  if (prc) {
    memcpy(&drawstate.ds_renderbase.rc, prc, sizeof(Rect));
  }
  drawstate.ds_renderbase.nState = nState;

  drawstate.szText = szText;
  drawstate.nDrawTextFlag = nDrawTextFlag;

  static_cast<IMessage *>(this)->SendMessage(UI_MSG_RENDERBASE_DRAWSTATE,
                                             (long)&drawstate, 0);
}
void ITextRenderBase::Init() {
  static_cast<IMessage *>(this)->SendMessage(UI_MSG_INITIALIZE);
}
void ITextRenderBase::CheckSkinTextureChanged() {
  return __pImpl->CheckSkinTextureChanged();
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

void ISimpleTextRender::SetColor(Color *pColText) {
  return __pImpl->SetColor(pColText);
}
void ISimpleTextRender::SetRenderFont(IRenderFont *p) {
  __pImpl->SetRenderFont(p);
}
bool ISimpleTextRender::GetColor(COLORREF &color) {
  return __pImpl->GetColor(color);
}

//////////////////////////////////////////////////////////////////////////

void IColorListTextRender::SetRenderFont(IRenderFont *p) {
  __pImpl->SetRenderFont(p);
}
void IColorListTextRender::SetCount(int nCount) { __pImpl->SetCount(nCount); }
void IColorListTextRender::SetColor(int nIndex, COLORREF col) {
  __pImpl->SetColor(nIndex, col);
}

//////////////////////////////////////////////////////////////////////////

void IFontColorListTextRender::SetCount(int nCount) {
  return __pImpl->SetCount(nCount);
}
void IFontColorListTextRender::SetColor(int nIndex, unsigned int color) {
  return __pImpl->SetColor(nIndex, color);
}
void IFontColorListTextRender::SetFont(int nIndex, IRenderFont *p) {
  return __pImpl->SetFont(nIndex, p);
}

} // namespace ui