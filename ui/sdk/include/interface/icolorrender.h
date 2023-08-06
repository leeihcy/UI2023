#ifndef _UI_ICOLORRENDER_H_
#define _UI_ICOLORRENDER_H_
#include "irenderbase.h"

namespace ui {

class ColorRender;
struct UIAPI IColorRender : public IRenderBase {
  void SetBkColor(COLORREF col);
  void SetBorderColor(COLORREF col);
  void SetBorderRegion(const Rect *prc);

  UI_DEFINE_CLASS_GUID("A3AE0787-D67E-40A2-8AFE-2B6783A55DAF");
  UI_DECLARE_INTERFACE(ColorRender);
};

//////////////////////////////////////////////////////////////////////////

class SysColorRender;
struct UIAPI ISysColorRender : public IRenderBase {
  void SetBkColor(int nColorIndex);
  void SetBorderColor(int nColorIndex);

  UI_DEFINE_CLASS_GUID("D9619AFA-291B-41CC-83FE-43CA5079A962");
  UI_DECLARE_INTERFACE(SysColorRender);
};

//////////////////////////////////////////////////////////////////////////

class GradientRender;
struct UIAPI IGradientRender : public IRenderBase {
  UI_DEFINE_CLASS_GUID("B7B00FDE-964F-4F67-B859-58EA62D8946E");
  UI_DECLARE_INTERFACE(GradientRender);
};

//////////////////////////////////////////////////////////////////////////

class ColorListRender;
struct UIAPI IColorListRender : public IRenderBase {
  void SetStateColor(int nState, COLORREF colorBk, bool bSetBk,
                     COLORREF colBorder, bool bSetBorder);
  void SetCount(int n);

  UI_DEFINE_CLASS_GUID("BB63B44B-831C-4BE0-9053-6FE62C88A5FF");
  UI_DECLARE_INTERFACE(ColorListRender);
};
} // namespace ui

#endif // _UI_ICOLORRENDER_H_