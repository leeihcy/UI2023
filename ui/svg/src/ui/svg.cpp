#include "svg.h"
#include "svg_layout.h"
#include "svg_meta.h"
#include "src/element/dom.h"

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
  

  static_cast<IPanel *>(m_pISvg)->onRouteMessage(msg);
}

void Svg::onFinalConstruct() {
}

void Svg::onPaint(ui::IRenderTarget *rt) {
  // elements:
  RenderContext context;
  context.canvas = (SkCanvas *)rt->GetHandle();
  context.paint.setAntiAlias(true);
  context.svg_size = { m_pISvg->GetWidth(), m_pISvg->GetHeight() };
  context.view_port_size = { 0, 0 };

  // 默认黑色
  context.paint.setColor(SkColorSetARGB(255, 0, 0, 0));
  
  if (m_root) {
    m_root->Render(context);
  }
}

void Svg::onEraseBkgnd(ui::IRenderTarget *rt) {
  // svg 背景
  Rect rc;
  m_pISvg->GetObjectClientRect(&rc);
  ui::Color c = ui::Color::Make(0xFFFFFFFF);
  rt->DrawRect(&rc, &c);
}

void Svg::SetAttribute(ui::SerializeParam& data) {}


bool Svg::Load(const char *stream) {
  if (m_root) {
    m_root.reset();
  }

  ::svg::Dom dom;
  m_root = dom.Parse(stream);
  return !!m_root;
}

} // namespace ui