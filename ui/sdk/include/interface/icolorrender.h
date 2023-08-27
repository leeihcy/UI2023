#ifndef _UI_ICOLORRENDER_H_
#define _UI_ICOLORRENDER_H_
#include "irenderbase.h"

namespace ui {

class ColorRender;
struct UIAPI IColorRender : public IRenderBase {
  void SetBkColor(Color col);
  void SetBorderColor(Color col);
  void SetBorderRegion(const Rect *prc);

  UI_DECLARE_INTERFACE(ColorRender);
};

//////////////////////////////////////////////////////////////////////////

class SysColorRender;
struct UIAPI ISysColorRender : public IRenderBase {
  void SetBkColor(int nColorIndex);
  void SetBorderColor(int nColorIndex);

  UI_DECLARE_INTERFACE(SysColorRender);
};

//////////////////////////////////////////////////////////////////////////

class GradientRender;
struct UIAPI IGradientRender : public IRenderBase {
  UI_DECLARE_INTERFACE(GradientRender);
};

//////////////////////////////////////////////////////////////////////////

class ColorListRender;
struct UIAPI IColorListRender : public IRenderBase {
  void SetStateColor(int nState, Color colorBk, bool bSetBk,
                     Color colBorder, bool bSetBorder);
  void SetCount(int n);

  UI_DECLARE_INTERFACE(ColorListRender);
};
} // namespace ui

#endif // _UI_ICOLORRENDER_H_