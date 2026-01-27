#include "html/css/css_property_value_set.h"

namespace html {

const CSSValue *CSSPropertyValueSet::GetPropertyValue(const CSSPropertyID &id) {
  std::vector<CSSPropertyValue>::iterator iter = m_property_vector.begin();
  for (; iter != m_property_vector.end(); ++iter) {
    if (iter->Id() == id) {
      return iter->Value();
    }
  }
  return nullptr;
}

const CSSValue *CSSPropertyValueSet::GetPropertyValue(const AtomicString &id) {
  std::vector<CSSPropertyValue>::iterator iter = m_property_vector.begin();
  for (; iter != m_property_vector.end(); ++iter) {
    if (iter->CustomPropertyName() == id) {
      return iter->Value();
    }
  }
  return nullptr;
}

} // namespace html
