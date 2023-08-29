#include "element.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Element::Render(RenderContext &context) {}
void Element::SetAttribute(ui::SerializeParam &data) {
  ui::AttributeSerializerWrap s(&data, "Element");
  s.AddColor("fill", m_fill);
  s.AddColor("stroke", m_stroke);
  s.AddLength("stroke-width", m_stroke_width)
      ->SetDefault(ui::Length::MakeNumber(1));
}

} // namespace svg