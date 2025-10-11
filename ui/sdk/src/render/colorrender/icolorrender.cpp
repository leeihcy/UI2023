#include "include/inc.h"
#include "include/interface/icolorrender.h"
#include "src/render/colorrender/colorrender.h"
#include "src/render/render_meta.h"

namespace ui
{
UI_IMPLEMENT_INTERFACE(ColorRender, RenderBase)
UI_IMPLEMENT_INTERFACE(GradientRender, RenderBase)
UI_IMPLEMENT_INTERFACE(ColorListRender, RenderBase)

void  IColorRender::SetBkColor(Color col)
{
    return __pImpl->SetBkColor(col);
}
void  IColorRender::SetBorderColor(Color col)
{
    return __pImpl->SetBorderColor(col);
}
void  IColorRender::SetBorder(int b)
{
    __pImpl->SetBorder(b);
}
//////////////////////////////////////////////////////////////////////////

void  IColorListRender::SetStateColor(int nState, Color colorBk, bool bSetBk, Color colBorder, bool bSetBorder)
{  
    return __pImpl->SetStateColor(nState, colorBk, bSetBk, colBorder, bSetBorder);
}
void  IColorListRender::SetCount(int n)
{
    __pImpl->SetCount(n);
}

}