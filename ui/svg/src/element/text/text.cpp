#include "text.h"
#include "sdk/include/interface/iattribute.h"
#include "SkFont.h"
#include "SkTextBlob.h"
#include "include/utils/SkTextUtils.h"

namespace svg {

void Text::Render(RenderContext &context) {
  int x = context.ResolveX(m_x);
  int y = context.ResolveY(m_y);

  if (m_fill.value) {
    context.paint.setColor(m_fill.value);
  }

/*
if (fill) {
            ctx.canvas()->drawTextBlob(blob, 0, 0, *fill);
        }
        if (stroke) {
            ctx.canvas()->drawTextBlob(blob, 0, 0, *stroke);
        }
*/
  SkFont font(nullptr, m_font_size);

  SkTextUtils::Draw(context.canvas, m_data.c_str(), m_data.length(),
                    SkTextEncoding::kUTF8, (SkScalar)x, (SkScalar)y, font,
                    context.paint, (SkTextUtils::Align)m_text_anchor);
}

void Text::SetDomData(const char* data) {
  if (!data) {
    return;
  }
  m_data = data;
}

void Text::SetAttribute(ui::SerializeParam &data) {
  Element::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Rect");
  s.AddLength("x", m_x);
  s.AddLength("y", m_y);
  s.AddInt("font-size", m_font_size); // TODO: default: medium

  s.AddEnum("text-anchor", m_text_anchor)
    ->AddOption(SkTextUtils::kLeft_Align, "start")
    ->AddOption(SkTextUtils::kCenter_Align, "middle")
    ->AddOption(SkTextUtils::kRight_Align, "end");
}

}