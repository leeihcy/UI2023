#ifndef _UI_SDK_SRC_PANEL_ROUND_PANEL_H_
#define _UI_SDK_SRC_PANEL_ROUND_PANEL_H_
#include "panel.h"
#include "src/graphics/bitmap/skiabitmap.h"

// 注：1. back.render的数据不会做圆角化，fore.render+child的数据会被圆角化
//    2. 实现原理：利用skia的anti clip path，直接支持抗锯齿的圆角裁剪。

namespace ui {

class RoundPanel : public Panel {
public:
  RoundPanel(IRoundPanel *);
  ~RoundPanel();

  void onRouteMessage(ui::Msg *msg);

public:
  void SetRadius(const CornerRadius& radius);
  void SetRadius(int radius);

protected:
  void onSerialize(SerializeParam *pData);
  void onPaintBkgnd(IRenderTarget *pRenderTarget);
  void onPaint(IRenderTarget *pRenderTarget);
  void onPostPaint(IRenderTarget *pRenderTarget);

  void loadCorner(CornerRadius* radius);
  void saveCorner(CornerRadius* radius);

  void prePaint(IRenderTarget *pRenderTarget, int width, int height);
  void postPaint(IRenderTarget *pRenderTarget, int width, int height);
private:
  IRoundPanel *m_pIRoundPanel;
  CornerRadius m_corner_radius;
};

} // namespace ui

#endif