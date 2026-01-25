#ifndef _HTML_CSS_CSSPROPERTYVALUE_H_
#define _HTML_CSS_CSSPROPERTYVALUE_H_

#include "html/css/css_property_name.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_value.h"

namespace html {


class CSSPropertyValue {
public:
  explicit CSSPropertyValue(const CSSPropertyName& name, A<CSSValue>&& value, bool important=false)
    : m_property_id(name.m_property_id), m_value(std::move(value)), m_important(important){
      if (m_property_id == CSSPropertyID::Variable) {
        m_custom_property_name = name.m_custom_property_name;
      }
  }
  CSSPropertyValue() {}

  CSSPropertyID Id() { return m_property_id; }
  const std::string& CustomPropertyName() { return m_custom_property_name;}
  const CSSValue* Value() { return m_value.get(); }
  bool IsImportant() { return m_important; }
  void SetImportant(bool important) { m_important = important; }
private:
  CSSPropertyID m_property_id;
  std::string m_custom_property_name;
  A<CSSValue> m_value = nullptr;
  bool m_important = false;
};


}

#endif