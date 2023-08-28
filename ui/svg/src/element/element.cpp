#include "element.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Element::Render(RenderContext& context) {

}
void Element::SetAttribute(ui::SerializeParam& data) {
  ui::AttributeSerializerWrap s(&data, "Element");
  s.AddColor("fill", m_fill);
}

}