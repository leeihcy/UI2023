#ifndef _UI_SVG_SRC_ELEMENT_TEXT_TEXT_H_
#define _UI_SVG_SRC_ELEMENT_TEXT_TEXT_H_

#include "src/element/element.h"
#include "sdk/include/util/rect.h"

namespace svg {

class Text : public Element {
public:
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;
  void SetDomData(const char* data) override;

private:
  ui::Length m_x;
  ui::Length m_y;
  std::string m_data;
  
  int m_font_size;
  int m_text_anchor;
};

}
#endif