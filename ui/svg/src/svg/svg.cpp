#include "svg.h"
#include "svg_desc.h"

namespace ui { namespace svg {

Svg::Svg(ISvg* p) : ui::MessageProxy(p) {
  m_pISvg = p;
  m_pISvg->SetDescription(SvgDescription::Get());
}

void Svg::OnEraseBkgnd(ui::IRenderTarget* rt)
{
}


}}