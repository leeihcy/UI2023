#ifndef _UI_SVG_SRC_ELEMENT_TEXT_TEXT_H_
#define _UI_SVG_SRC_ELEMENT_TEXT_TEXT_H_

#include "sdk/include/util/rect.h"
#include "src/element/element.h"

namespace svg {

class Text : public Element {
public:
  void Render(RenderContext &context) override;
  void SetAttribute(ui::SerializeParam &data) override;
  void SetDomData(const char *data) override;

private:
  ui::Length m_x;
  ui::Length m_y;
  std::string m_data;

  // This property indicates the desired height of glyphs from the font. For
  // scalable fonts, the font-size is a scale factor applied to the EM unit of
  // the font. (Note that certain glyphs may bleed outside their EM box.) For
  // non-scalable fonts, the font-size is converted into absolute units and
  // matched against the declared ‘font-size’ of the font, using the same
  // absolute coordinate space for both of the matched values. Values have the
  // following meanings:
  int m_font_size;
  int m_text_anchor;
};

} // namespace svg
#endif