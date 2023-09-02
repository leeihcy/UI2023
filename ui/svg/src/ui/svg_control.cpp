#include "svg_control.h"
#include "svg_layout.h"
#include "svg_meta.h"
#include "src/element/dom.h"

#include "SkData.h"
#include "SkBitmap.h"
#include "SkFont.h"
#include "SkImage.h"
#include "SkTextBlob.h"
#include "SkRRect.h"
#include "include/utils/SkTextUtils.h"

namespace ui {

Svg::Svg(ISvg *p) : ui::MessageProxy(p) {
  m_pISvg = p;
  p->SetLayout(new SvgLayout(p));
}

void Svg::onRouteMessage(Msg *msg) {
  if (msg->message == UI_MSG_ERASEBKGND) {
    static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
    onEraseBkgnd(static_cast<EraseBkgndMessage *>(msg)->rt);
    return;
  }
  if (msg->message == UI_MSG_PAINT) {
    static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
    onPaint(static_cast<PaintMessage *>(msg)->rt);
    return;
  }
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
    onFinalConstruct();
    return;
  }
  if (msg->message == UI_MSG_GETDESIREDSIZE) {
    static_cast<GetDesiredSizeMessage*>(msg)->size = onGetDesiredSize();
    return;
  }

  static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
}

void Svg::onFinalConstruct() {
}

void Svg::onPaint(ui::IRenderTarget *rt) {

#if 0
  SkCanvas* canvas = (SkCanvas *)rt->GetHandle();

  SkPaint paint;
  paint.setAntiAlias(true);
  paint.setColor(0xFF0000FF);

  SkRect rect = SkRect::MakeXYWH(30, 30, 400, 400);
  canvas->drawRRect(SkRRect::MakeRectXY(rect, 50, 50), paint);


  int m_font_size = 200;
  SkFont font(nullptr, m_font_size);

  std::string m_data = "SVG";
  float x = 250;
  float y = 200;
 
  SkTextUtils::Draw(canvas, m_data.c_str(), m_data.length(),
                    SkTextEncoding::kUTF8, (SkScalar)x, (SkScalar)y, font,
                    paint, SkTextUtils::Align::kCenter_Align);
#endif
  // const char* file_path = "/Users/libo/2030/github/UI2023/ui/svg/test/test.svg";
  // SkBitmap image2;
  // SkImageDecoder::DecodeFile("/Users/libo/2030/github/UI2023/ui/svg/test/test.svg", &image2);
	// canvas->drawBitmap(image2,10,10,NULL);

  // sk_sp<SkData> data(SkData::MakeFromFileName(file_path));
  // sk_sp<SkImage> image =SkImage::MakeFromEncoded(data);
  // canvas->drawImage(image, 0, 0);
#if 1
  // elements:
  RenderContext context;
  context.canvas = (SkCanvas *)rt->GetHandle();
  context.canvas_size = { m_pISvg->GetWidth(), m_pISvg->GetHeight() };
  
  // 线条抗锯齿
  context.paint.setAntiAlias(true);

  // 默认黑色
  context.paint.setColor(SkColorSetARGB(255, 0, 0, 0));
  
  if (m_root) {
    m_root->Render(context);
  }
#endif
}

void Svg::onEraseBkgnd(ui::IRenderTarget *rt) {
  // svg 背景
  Rect rc;
  m_pISvg->GetObjectClientRect(&rc);
  ui::Color c = ui::Color::Make(0xFFFFFFFF);
  rt->DrawRect(&rc, &c);
}

ui::Size Svg::onGetDesiredSize() {
  Size size = {0, 0};
  if (!m_root) {
    return size;
  }

  return m_root->GetDesiredSize();
}

void Svg::SetAttribute(ui::SerializeParam& data) {}


bool Svg::Load(const char *stream) {
  if (m_root) {
    m_root.reset();
  }

  ::svg::Dom dom;
  m_root = dom.Parse(stream);

  m_pISvg->UpdateLayout();
  return !!m_root;
}

} // namespace ui