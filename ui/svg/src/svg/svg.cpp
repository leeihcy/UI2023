#include "svg.h"
#include "svg_meta.h"
#include "svg_layout.h"

namespace ui { namespace svg {

Svg::Svg(ISvg* p) : ui::MessageProxy(p) {
  m_pISvg = p;
  p->SetLayout(new SvgLayout());
}

void Svg::OnEraseBkgnd(ui::IRenderTarget* rt)
{

}


}}