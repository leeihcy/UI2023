#ifndef _UI_SDK_SRC_RENDER_COLORREDNER_COLORRENDER_H_
#define _UI_SDK_SRC_RENDER_COLORREDNER_COLORRENDER_H_

#include "include/interface/icolorrender.h"
#include "include/util/color.h"
#include "include/util/struct.h"
#include "src/render/renderbase.h"
#include <vector>

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
  void SetBackColor(Color col);
  void SetBorderColor(Color col);
  void SetBackColor(Color color, int index);
  void SetBorderColor(Color color, int index);
  
  void SetBorder(int b);

protected:
  void OnSerialize(SerializeParam *pData);
  void DrawState(RENDERBASE_DRAWSTATE *pDrawStruct);

  void LoadBackColor(const char *color);
  const char *SaveBackColor();
  void LoadBorderColor(const char *color);
  const char *SaveBorderColor();

  void loadColor(const char *text, std::vector<Color>& target);
  const char * saveColor(std::vector<Color>& target);

public:
  IColorRender *m_pIColorRender;

  // 同时兼容单个颜色，和多个颜色的场景。不再区分为两个类。
  std::vector<Color> m_back_color;
  std::vector<Color> m_border_color;

  int m_border = 0;
  CornerRadius  m_radius;
};

} // namespace ui

#endif