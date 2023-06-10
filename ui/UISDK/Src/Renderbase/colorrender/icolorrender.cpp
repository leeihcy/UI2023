#include "stdafx.h"
#include "Inc\Interface\icolorrender.h"
#include "Src\Renderbase\colorrender\colorrender.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE(ColorRender, RenderBase)
UI_IMPLEMENT_INTERFACE(SysColorRender, RenderBase)
UI_IMPLEMENT_INTERFACE(GradientRender, RenderBase)
UI_IMPLEMENT_INTERFACE(ColorListRender, RenderBase)

void  IColorRender::SetBkColor(COLORREF col)
{
    return __pImpl->SetBkColor(col);
}
void  IColorRender::SetBorderColor(COLORREF col)
{
    return __pImpl->SetBorderColor(col);
}
void  IColorRender::SetBorderRegion(const RECT* prc)
{
    __pImpl->SetBorderRegion(prc);
}
//////////////////////////////////////////////////////////////////////////

void  IColorListRender::SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder)
{  
    return __pImpl->SetStateColor(nState, colorBk, bSetBk, colBorder, bSetBorder);
}
void  IColorListRender::SetCount(int n)
{
    __pImpl->SetCount(n);
}

void  ISysColorRender::SetBkColor(int nColorIndex)
{
	__pImpl->SetBkColor(nColorIndex);
}
void  ISysColorRender::SetBorderColor(int nColorIndex)
{
	__pImpl->SetBorderColor(nColorIndex);
}

}