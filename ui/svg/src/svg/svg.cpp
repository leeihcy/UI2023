#include "svg.h"
#include "svg_layout.h"
#include "svg_meta.h"

// #include "SkStream.h"
// #include "SkSVGDOM.h"
#include "src/element/circle/circle.h"
#include "src/element/ellipse/ellipse.h"
#include "src/element/line/line.h"
#include "src/element/rect/rect.h"

namespace ui {
namespace svg {

Svg::Svg(ISvg *p) : ui::MessageProxy(p) {
  m_pISvg = p;
  p->SetLayout(new SvgLayout(p));
}

void Svg::onRouteMessage(Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
    onFinalConstruct();
    return;
  }
  if (msg->message == UI_MSG_ERASEBKGND) {
    static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
    onEraseBkgnd(static_cast<EraseBkgndMessage *>(msg)->rt);
    return;
  }

  static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
}

void Svg::onFinalConstruct() {
  // test:
  // auto rect = std::make_unique<::Rect>();
  // rect->SetAttribute("x", "10");
  // rect->SetAttribute("y", "10");
  // rect->SetAttribute("width", "200");
  // rect->SetAttribute("height", "80");
  // rect->SetAttribute("rx", "20");
  // rect->SetAttribute("ry", "20");
  // m_root.AddChild(std::move(rect));

  // auto circle = std::make_unique<::Circle>();
  // circle->SetAttribute("cx", "100");
  // circle->SetAttribute("cy", "100");
  // circle->SetAttribute("r", "50");
  // m_root.AddChild(std::move(circle));

  // auto ellipse = std::make_unique<::Ellipse>();
  // ellipse->SetAttribute("cx", "110");
  // ellipse->SetAttribute("cy", "180");
  // ellipse->SetAttribute("rx", "100");
  // ellipse->SetAttribute("ry", "50");
  // m_root.AddChild(std::move(ellipse));

  // auto line = std::make_unique<::Line>();
  // line->SetAttribute("x1", "5");
  // line->SetAttribute("y1", "10");
  // line->SetAttribute("x2", "5");
  // line->SetAttribute("y2", "400");
  // m_root.AddChild(std::move(line));
}

void Svg::onEraseBkgnd(ui::IRenderTarget *rt) {
  // svg 背景
  Rect rc;
  m_pISvg->GetObjectClientRect(&rc);
  ui::Color c(0xFFFFFFFF);
  rt->DrawRect(&rc, &c);

  // elements:
  RenderContext context;
  context.canvas = (SkCanvas *)rt->GetHandle();
  context.paint.setAntiAlias(true);

  // 默认黑色
  context.paint.setColor(SkColorSetARGB(255, 0, 0, 0));

  m_root.Render(context);

  // SkFILEStream stream("/Users/libo/2030/github/demo.svg");
  // if (!stream.isValid()) {
  //   return;
  // }
  // // SkMemoryStream stream{blob};
  // sk_sp<SkSVGDOM> svg = SkSVGDOM::MakeFromStream(stream);
  // if (!svg) {
  //   return;
  // }
}

bool Svg::Load(const char *stream) { return false; }

} // namespace svg
} // namespace ui