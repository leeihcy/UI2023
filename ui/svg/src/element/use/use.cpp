#include "use.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Use::PrepareRender(RenderContext &ctx) {
  if (m_href.empty()) {
    return;
  }

  Element *target = GetRootElement()->FindElementById(m_href.c_str());
  if (!target) {
    return;
  }

  target->PrepareRender(ctx);
  Element::PrepareRender(ctx);

  int x = ctx.ResolveX(m_x);
  int y = ctx.ResolveY(m_y);
  if (x == 0 && y == 0) {
    return;
  }

  SkMatrix matrix;
  matrix.setTranslate(x, y);
  ctx.matrix.preConcat(matrix);

  ctx.canvas->setMatrix(ctx.matrix);
}

void Use::Render(RenderContext &ctx) {
  if (m_href.empty()) {
    return;
  }

  Element *target = GetRootElement()->FindElementById(m_href.c_str());
  if (!target) {
    return;
  }

  target->Render(ctx);
}

void Use::setHref(const char *text) {
  if (!text) {
    m_href.clear();
    return;
  }
  if (text[0] == '#') {
    m_href = text + 1;
  } else {
    m_href = text;
  }
}

const char *Use::getHref() {
  // not implement
  return nullptr;
}

void Use::SetAttribute(ui::SerializeParam &data) {
  Element::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Use");
  s.AddLength("x", m_x);
  s.AddLength("y", m_y);
  s.AddString("href", ui::Slot(&Use::setHref, this),
              ui::Slot(&Use::getHref, this));
}

} // namespace svg