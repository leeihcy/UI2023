#ifndef _HTML_CSS_CSSPROPERTYVALUESET_H_
#define _HTML_CSS_CSSPROPERTYVALUESET_H_

#include "html/css/css_property_value.h"
#include <vector>

namespace html {


class CSSPropertyValueSet {
public:
  CSSPropertyValueSet(std::vector<CSSPropertyValue> &&array)
      : m_property_vector(std::move(array)) {}

  size_t Size() { return m_property_vector.size(); }
  CSSPropertyValue &Item(unsigned int i) {
    if (i >= m_property_vector.size()) {
      abort();
    }
    return m_property_vector[i];
  };

private:
  std::vector<CSSPropertyValue> m_property_vector;
};


}

#endif