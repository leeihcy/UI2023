#include "include/inc.h"
#include "include/interface/irenderbase.h"
#include "src/render/renderbase.h"
#include "src/object/object.h"
#include "include/interface/iuiapplication.h"
#include "src/resource/colorres.h"
#include "src/resource/imageres.h"
#include "src/render/render_meta.h"

namespace ui
{
UI_IMPLEMENT_INTERFACE(RenderBase, Message)
// UI_IMPLEMENT_INTERFACE(NullRender, RenderBase)


void  IRenderBase::CheckThemeChanged()
{
    return __pImpl->CheckThemeChanged();
}

bool  IRenderBase::IsThemeRender()
{
    return __pImpl->IsThemeRender();
}
void  IRenderBase::SetObject(IApplication*  pUIApp, IObject* pObject)
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

void  IRenderBase::Serialize(SerializeParam* pData)
{
    static_cast<IMessage*>(this)->SendMessage(UI_MSG_SERIALIZE, (long)pData);
}

void  IRenderBase::DrawState(IRenderTarget* p, const Rect* prc, int nState)
{
    RENDERBASE_DRAWSTATE drawstate = {0};
    drawstate.pRenderTarget = p;
    if (prc) {
        drawstate.rc.CopyFrom(*prc);
    }
    drawstate.nState = nState;

    static_cast<IMessage*>(this)->SendMessage(
      UI_MSG_RENDERBASE_DRAWSTATE, (long)&drawstate, 0);
};
Size  IRenderBase::GetDesiredSize() 
{ 
    Size s = {0,0}; 
    static_cast<IMessage*>(this)->SendMessage(
      UI_MSG_GETDESIREDSIZE, (long)&s, 0);

    return s; 
}
void  IRenderBase::Init()
{
    static_cast<IMessage*>(this)->SendMessage(UI_MSG_INITIALIZE);
}
}