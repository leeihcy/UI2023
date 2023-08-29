#include "path.h"
#include "include/utils/SkParsePath.h"
#include "sdk/include/interface/iattribute.h"
#include "sdk/include/interface/imapattr.h"

namespace svg {

void Path::Render(RenderContext &context) {
  context.canvas->drawPath(m_path, context.paint);
}

void Path::SetAttribute(ui::SerializeParam &data) {
  Shape::SetAttribute(data);

  const char *text = data.pMapAttrib->GetAttr("d", true);
  if (text) {
    SkParsePath::FromSVGString(text, &m_path);
  }
}

} // namespace svg