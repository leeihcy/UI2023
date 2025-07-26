#pragma once
// #include "Src\Atl\image.h"
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
  void SetRadius(int lefttop, int righttop, int leftbottom, int rightbottom);

protected:
  void onSerialize(SerializeParam *pData);
  void onPaintBkgnd(IRenderTarget *pRenderTarget);
  void onPaint(IRenderTarget *pRenderTarget);
  void onPostPaint(IRenderTarget *pRenderTarget);

  void loadCorner(Rect *);
  void saveCorner(Rect *);

  void prePaint(IRenderTarget *pRenderTarget, int width, int height);
  void postPaint(IRenderTarget *pRenderTarget, int width, int height);
private:
  IRoundPanel *m_pIRoundPanel;
  Rect m_corner;
};

} // namespace ui