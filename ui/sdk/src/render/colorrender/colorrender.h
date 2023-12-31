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
  void SetBorderRegion(const Rect *prc);

protected:
  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);

public:
  IColorRender *m_pIColorRender;

  Color m_back_color;
  Color m_border_color;
  Rect m_rcBorder;
};

// 特用于某些控件需要使用系统主题颜色背景，当系统主题改变时，相应的颜色也需要改变
class SysColorRender : public /*Theme*/ RenderBase {
public:
  SysColorRender(ISysColorRender *p);
  ~SysColorRender();

  // UI_BEGIN_MSG_MAP()
  //     UIMSG_RENDERBASE_DRAWSTATE(DrawState)
  //     UIMSG_QUERYINTERFACE(SysColorRender)
  //     UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);

  void SetBkColor(int nColorIndex);
  void SetBorderColor(int nColorIndex);

  // 由于改成每次重新获取颜色值，因此不需要响应主题改变的消息
  // virtual const char* GetThemeName() { return nullptr; }
  // virtual void  OnThemeChanged();

public:
  ISysColorRender *m_pISysColorRender;
  int m_nBkColorIndex;
  int m_nBorderColorIndex;
  REGION4 m_rcBorder;

  // Color    m_bkColor;     //
  // 由于在WM_THEMECHANGED消息中，直接调用GetSysColor获取到的值仍然有可能是旧的
  // Color    m_borderColor; //
  // 因此这里放弃保存颜色值，而是每次绘制时都调用GetSysColor
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

  void LoadFromColor(const char *sz) { _LoadColor(sz, m_pColorFrom); }
  void LoadToColor(const char *sz) { _LoadColor(sz, m_pColorTo); }
  void LoadBorderColor(const char *sz) { _LoadColor(sz, m_pBorderColor); }
  const char *SaveFromColor() { return _GetColorId(m_pColorFrom); }
  const char *SaveToColor() { return _GetColorId(m_pColorTo); }
  const char *SaveBorderColor() { return _GetColorId(m_pBorderColor); }

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

  // UI_BEGIN_MSG_MAP()
  //     UIMSG_RENDERBASE_DRAWSTATE(DrawState)
  //     UIMSG_QUERYINTERFACE(ColorListRender)
  //     UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

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
  std::vector<Color *> m_vBkColor;
  std::vector<Color *> m_vBorderColor;
  int m_nCount;
};
} // namespace ui