#include "inc.h"
#include "include/interface/itextrenderbase.h"
#include "src/render/textrender/textrender.h"
#include "src/object/object.h"
#include "include/interface/iuiapplication.h"

namespace ui
{

UI_IMPLEMENT_INTERFACE(TextRenderBase, Message)
// UI_IMPLEMENT_INTERFACE(ThemeTextRenderBase, TextRenderBase)
UI_IMPLEMENT_INTERFACE(SimpleTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(ContrastColorTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(ContrastColorListTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(ColorListTextRender, TextRenderBase)
UI_IMPLEMENT_INTERFACE(FontColorListTextRender, TextRenderBase)

ITextRenderBase::ITextRenderBase() : IMessage(CREATE_IMPL_FALSE)
{
	m_pImpl = new TextRenderBase(this);
}

long  ITextRenderBase::AddRef() 
{ 
	return __pImpl->AddRef();
}
long  ITextRenderBase::Release()
{ 
	return __pImpl->Release();
}
void  ITextRenderBase::SetObject(IObject* pObject)
{
    Object* pObj = nullptr;
    if (pObject)
        pObj = pObject->GetImpl();

    __pImpl->SetObject(pObj);
}
IObject*  ITextRenderBase::GetObject()
{
    Object* pObj = __pImpl->GetObject();
    if (pObj)
        return pObj->GetIObject();
    return nullptr;
}
void  ITextRenderBase::SetTextAlignment(int nDrawFlag)
{
    return __pImpl->SetTextAlignment(nDrawFlag);
}
int  ITextRenderBase::GetTextAlignment()
{
    return __pImpl->GetTextAlignment();
}
void  ITextRenderBase::SetTextEffect(TEXT_EFFECT e)
{
    __pImpl->SetTextEffect(e);
}
TEXT_EFFECT  ITextRenderBase::GetTextEffect()
{
    return __pImpl->GetTextEffect();
}
void  ITextRenderBase::SetDrawTextParam(long w, long l)
{
    __pImpl->SetDrawTextParam(w, l);
}
void  ITextRenderBase::SetType(const TEXTRENDER_TYPE& nType)
{
    __pImpl->SetTextRenderType(nType);
}
TEXTRENDER_TYPE  ITextRenderBase::GetType()
{
    return __pImpl->GetTextRenderType();
}

IRenderFont*  ITextRenderBase::GetRenderFont()
{
    return (IRenderFont*)UISendMessage(static_cast<IMessage*>(this),
        UI_MSG_GETRENDERFONT);
}

void  ITextRenderBase::Serialize(SERIALIZEDATA* pData)
{
    UISendMessage(static_cast<IMessage*>(this), UI_MSG_SERIALIZE, (long)pData);
}

void  ITextRenderBase::_LoadFont(const wchar_t* szFontId, IRenderFont*& pRenderFont)
{
	__pImpl->_LoadFont(szFontId, pRenderFont);
}
const wchar_t*  ITextRenderBase::_SaveFont(IRenderFont*& pRenderFont)
{	
	return __pImpl->_SaveFont(pRenderFont);
}
void  ITextRenderBase::_LoadDefalutFont(IRenderFont** ppRenderFont)
{
	__pImpl->_LoadDefalutFont(ppRenderFont);
}
void  ITextRenderBase::_LoadColor(const wchar_t* szColorId, Color*& pColor)
{
	__pImpl->_LoadColor(szColorId, pColor);
}
const wchar_t*  ITextRenderBase::_SaveColor(Color*& pColor)
{
	return __pImpl->_SaveColor(pColor);
}

SIZE  ITextRenderBase::GetDesiredSize(const wchar_t* szText, int nLimitWidth)
{
//     SIZE s = {0,0}; 
// 
//     GETDESIREDSIZEINFO info;
//     info.nLimitWidth = nLimitWidth;
//     info.szText = szText;
//     UISendMessage(static_cast<IMessage*>(this), UI_WM_GETDESIREDSIZE, (long)&s, (long)&info);
// 
//     return s; 

    return __pImpl->GetDesiredSize(szText, nLimitWidth);
}
void  ITextRenderBase::DrawState(
        IRenderTarget* pRenderTarget, 
		const RECT* prc,
        int nState, 
        const wchar_t* szText, 
        int nDrawTextFlag)
{
    TEXTRENDERBASE_DRAWSTATE drawstate = {0};
    drawstate.ds_renderbase.pRenderTarget = pRenderTarget;
    if (prc) {
        memcpy(&drawstate.ds_renderbase.rc, prc, sizeof(RECT));
    }
    drawstate.ds_renderbase.nState = nState;

    drawstate.szText = szText;
    drawstate.nDrawTextFlag = nDrawTextFlag; 

    UISendMessage(static_cast<IMessage*>(this), 
        UI_MSG_RENDERBASE_DRAWSTATE, (long)&drawstate, 0);
}
void  ITextRenderBase::Init()
{
     UISendMessage(static_cast<IMessage*>(this), UI_MSG_INITIALIZE);
}
void  ITextRenderBase::CheckSkinTextureChanged()
{
    return __pImpl->CheckSkinTextureChanged();
}


// 
// const wchar_t*  IThemeTextRenderBase::GetThemeName()
// {
//     const wchar_t* pszName = nullptr;
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

void  ISimpleTextRender::SetColor(Color* pColText) 
{
    return __pImpl->SetColor(pColText); 
}
void  ISimpleTextRender::SetRenderFont(IRenderFont* p) 
{
    __pImpl->SetRenderFont(p);
}
bool  ISimpleTextRender::GetColor(COLORREF& color)
{
	return __pImpl->GetColor(color); 
}

//////////////////////////////////////////////////////////////////////////

void  IColorListTextRender::SetRenderFont(IRenderFont* p) 
{
    __pImpl->SetRenderFont(p);
}
void  IColorListTextRender::SetCount(int nCount)
{
    __pImpl->SetCount(nCount); 
}
void  IColorListTextRender::SetColor(int nIndex, COLORREF col)
{
    __pImpl->SetColor(nIndex, col); 
}

//////////////////////////////////////////////////////////////////////////

void  IFontColorListTextRender::SetCount(int nCount)
{
    return __pImpl->SetCount(nCount);
}
void  IFontColorListTextRender::SetColor(int nIndex, unsigned int color) 
{ 
    return __pImpl->SetColor(nIndex, color);
}
void  IFontColorListTextRender::SetFont(int nIndex, IRenderFont* p) 
{
    return __pImpl->SetFont(nIndex, p); 
}

}