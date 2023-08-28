#include "svg.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Svg::Render(RenderContext &context) {
  Container::Render(context);
}

void Svg::SetAttribute(ui::SerializeParam &data) {
  Container::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Svg");
  s.AddLength("width", m_width)->SetDefault({100, ui::Length::Unit::Percentage});
  s.AddLength("height", m_height)->SetDefault({100, ui::Length::Unit::Percentage});
}

}