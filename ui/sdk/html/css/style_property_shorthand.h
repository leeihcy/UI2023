#ifndef _HTML_CSS_STYLE_PROPERTY_SHORTHAND_H_
#define _HTML_CSS_STYLE_PROPERTY_SHORTHAND_H_

#include "html/css/property/property_id.h"
#include "html/css/property/css_property.h"

namespace html {

// 描述一个短样式中，包含的所有长样式集合。
class StylePropertyShorthand {
 public:
  using Properties = const CSSProperty**;

  constexpr StylePropertyShorthand() : m_shorthand_id(CSSPropertyId::Invalid) {}

  constexpr StylePropertyShorthand(CSSPropertyId id, Properties properties, unsigned int size)
      : m_properties(properties), m_size(size), m_shorthand_id(id) {
  }

  const Properties& properties() const { return m_properties; }
  const CSSProperty& properties(unsigned int i) const { return *m_properties[i]; }
  unsigned int length() const { return static_cast<unsigned>(m_size); }
  CSSPropertyId id() const { return m_shorthand_id; }

 private:
  Properties m_properties = nullptr;
  unsigned int m_size = 0;

  CSSPropertyId m_shorthand_id;
};

const StylePropertyShorthand& backgroundShorthand();

}

#endif
