#include "include/interface/ipanel.h"
#include "src/panel/panel.h"
// #include "round_panel.h"

namespace ui
{
UI_IMPLEMENT_INTERFACE(Panel, Object)
// UI_IMPLEMENT_INTERFACE(RoundPanel, Panel)

ILayout* IPanel::GetLayout()                 
{
    return __pImpl->GetLayout(); 
}
void  IPanel::SetLayoutType(LAYOUTTYPE eLayoutType)  
{ 
    __pImpl->SetLayoutType(eLayoutType);
}
void  IPanel::SetTextureRender(IRenderBase* p)
{
    __pImpl->SetTextureRender(p);
}
IRenderBase*  IPanel::GetTextureRender()     
{ 
    return __pImpl->GetTextureRender();
}
}