#ifndef _UI_SDK_SRC_CONTROL_IMAGE_IMAGE_H_
#define _UI_SDK_SRC_CONTROL_IMAGE_IMAGE_H_

#include "include/interface/graphics.h"
#include "src/control/control.h"

namespace ui {

// 图片控件
// 与 Back Render 中的 ImageRender的区别：
// 1. 模拟 html <img> 控件，只需要配置一个 src 属性即可
// 2. 在 src 的基础上，仍然支持保留back/fore render做为补充。
// 
// TODO: dpi @percent support
//

class Image : public Control {
public:
  Image(IImage *p);

  void onRouteMessage(ui::Msg *msg);
  IImage *GetIImage() { return m_pIImage; }

private:
  void onFinalConstruct(FinalConstructMessage* msg);
  void onPaint(IRenderTarget* rt);
  void onSerialize(SerializeParam *pData);
  void onGetDesiredSize(Size *pSize);

private:
  void loadSrc(const char* src);
  const char*  saveSrc();

private:
  IImage *m_pIImage;

  std::string m_src;
  std::shared_ptr<IRenderBitmap> m_bitmap;
};

} // namespace ui

#endif