#pragma once
#include "include/interface/icolorrender.h"
#include "include/util/struct.h"
#include "src/render/renderbase.h"

namespace ui {

//
// 背景颜色+边框颜色（两者都是可选的）
//
class ColorRender : public RenderBase {
public:
  ColorRender(IColorRender *p);
  ~ColorRender();

  void onRouteMessage(ui::Msg *msg);

public:
  void SetBkColor(Color col);
  void SetBorderColor(Color col);
  void SetBorder(int b);

protected:
  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);

public:
  IColorRender *m_pIColorRender;

  Color m_back_color;
  Color m_border_color;
  int m_border = 0;
  CornerRadius  m_radius;
};

// 横向渐变颜色背景
class GradientRender : public RenderBase {
public:
  GradientRender(IGradientRender *p);
  ~GradientRender();

  // UI_BEGIN_MSG_MAP()
  //     UIMSG_RENDERBASE_DRAWSTATE(DrawState)
  //     UIMSG_QUERYINTERFACE(GradientRender)
  //     UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);

public:
  IGradientRender *m_pIGradientRender;
  Color *m_pColorFrom;
  Color *m_pColorTo;
  Color *m_pBorderColor;
  REGION4 m_rcBorder;

  int m_nGradientDir;
};

//
//	直接使用颜色值作为背景
//
class ColorListRender : public RenderBase {
public:
  ColorListRender(IColorListRender *p);
  ~ColorListRender();

  void onRouteMessage(ui::Msg *msg);
public:
  void Clear();

  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);

  void SetStateColor(int nState, Color colorBk, bool bSetBk, Color colBorder,
                     bool bSetBorder);

  void SetCount(int n);
  int GetCount();
  void LoadBkColor(const char *szColor);
  const char *SaveBkColor();
  void LoadBorderColor(const char *szColor);
  const char *SaveBorderColor();

private:
  IColorListRender *m_pIColorListRender;

  std::vector<Color> m_vBkColor;
  std::vector<Color> m_vBorderColor;
  int m_nCount;

  int m_border = 0;
  CornerRadius  m_radius;
};
} // namespace ui