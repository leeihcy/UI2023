#ifndef _UI_ICOLORRENDER_H_
#define _UI_ICOLORRENDER_H_
#include "irenderbase.h"

namespace UI
{

class ColorRender;
interface UIAPI_UUID(A3AE0787-D67E-40A2-8AFE-2B6783A55DAF) IColorRender
 : public IRenderBase
{
    void  SetBkColor(COLORREF col);
    void  SetBorderColor(COLORREF col);
    void  SetBorderRegion(const RECT* prc);

	UI_DECLARE_INTERFACE(ColorRender);
};

//////////////////////////////////////////////////////////////////////////

class SysColorRender;
interface UIAPI_UUID(D9619AFA-291B-41CC-83FE-43CA5079A962) ISysColorRender
 : public IRenderBase
{
    void  SetBkColor(int nColorIndex);
    void  SetBorderColor(int nColorIndex);

	UI_DECLARE_INTERFACE(SysColorRender);
};

//////////////////////////////////////////////////////////////////////////

class GradientRender;
interface UIAPI_UUID(B7B00FDE-964F-4F67-B859-58EA62D8946E) IGradientRender
 : public IRenderBase
{
    UI_DECLARE_INTERFACE(GradientRender);
};

//////////////////////////////////////////////////////////////////////////

class ColorListRender;
interface UIAPI_UUID(BB63B44B-831C-4BE0-9053-6FE62C88A5FF) IColorListRender
 : public IRenderBase
{
    void  SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder);
    void  SetCount(int n);

	UI_DECLARE_INTERFACE(ColorListRender);
};
}

#endif // _UI_ICOLORRENDER_H_