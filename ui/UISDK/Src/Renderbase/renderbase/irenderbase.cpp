#include "stdafx.h"
#include "Inc\Interface\irenderbase.h"
#include "Src\Renderbase\renderbase\renderbase.h"
#include "Src\Base\Object\object.h"
#include "Inc\Interface\iuiapplication.h"
#include "Src\Resource\colorres.h"
#include "Src\Resource\imageres.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE(RenderBase, Message)
UI_IMPLEMENT_INTERFACE(NullRender, RenderBase)

long  IRenderBase::AddRef() 
{ 
	return __pImpl->AddRef();
}
long  IRenderBase::Release()
{ 
	return __pImpl->Release();
}

void  IRenderBase::CheckThemeChanged()
{
    return __pImpl->CheckThemeChanged();
}

bool  IRenderBase::IsThemeRender()
{
    return __pImpl->IsThemeRender();
}
void  IRenderBase::SetObject(IUIApplication*  pUIApp, IObject* pObject)
{
    Object* pObj = nullptr;
    if (pObject)
    {
        pObj = pObject->GetImpl();
    }
    return __pImpl->SetObject(pUIApp?pUIApp->GetImpl():nullptr, pObj);
}
IObject*  IRenderBase::GetObject()
{
    Object* pObj = __pImpl->GetObject();
    if (nullptr == pObj)
        return nullptr;

    return pObj->GetIObject();
}
void  IRenderBase::SetType(RENDER_TYPE nType)
{
    __pImpl->SetRenderType(nType);
}
RENDER_TYPE  IRenderBase::GetType()
{
    return __pImpl->GetRenderType();
}
IColorRes*  IRenderBase::GetSkinColorRes()
{
    ColorRes* p = __pImpl->GetSkinColorRes();
    if (p)
        return &p->GetIColorRes();

    return nullptr;
}
IImageRes*  IRenderBase::GetSkinImageRes()
{
    ImageRes* p = __pImpl->GetSkinImageRes();
    if (p)
        return &p->GetIImageRes();

    return nullptr;
}

void  IRenderBase::Serialize(SERIALIZEDATA* pData)
{
    UISendMessage(static_cast<IMessage*>(this), UI_MSG_SERIALIZE, (WPARAM)pData);
}

void  IRenderBase::DrawState(IRenderTarget* p, LPCRECT prc, int nState)
{
    RENDERBASE_DRAWSTATE drawstate = {0};
    drawstate.pRenderTarget = p;
    if (prc)
        CopyRect(&drawstate.rc, prc);
    drawstate.nState = nState;

    UISendMessage(static_cast<IMessage*>(this), UI_MSG_RENDERBASE_DRAWSTATE, (WPARAM)&drawstate, 0);
};
SIZE  IRenderBase::GetDesiredSize() 
{ 
    SIZE s = {0,0}; 
    UISendMessage(static_cast<IMessage*>(this), UI_MSG_GETDESIREDSIZE, (WPARAM)&s, 0);

    return s; 
}
void  IRenderBase::Init()
{
    UISendMessage(static_cast<IMessage*>(this), UI_MSG_INITIALIZE);
}
}