#ifndef _HTML_CSS_CSSPROPERTYVALUESET_H_
#define _HTML_CSS_CSSPROPERTYVALUESET_H_

#include "html/base/atomic_string.h"
#include "html/css/css_property_value.h"
#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
#include <vector>

namespace html {


class CSSPropertyValueSet {
public:
  CSSPropertyValueSet(std::vector<CSSPropertyValue> &&array)
      : m_property_vector(std::move(array)) {}

  size_t Size() const { return m_property_vector.size(); }
  size_t Count() const { return Size(); }
  CSSPropertyValue &Item(unsigned int i) {
    if (i >= m_property_vector.size()) {
      abort();
    }
    return m_property_vector[i];
  };

  const CSSValue* GetPropertyValue(const CSSPropertyID& id);
  const CSSValue* GetPropertyValue(const AtomicString& id);
  
private:
  std::vector<CSSPropertyValue> m_property_vector;
};


}

#endif