#ifndef _UI_SDK_HTML_CSS_CSSPROPERTYNAME_H_
#define _UI_SDK_HTML_CSS_CSSPROPERTYNAME_H_

#include "html/base/atomic_string.h"
#include "html/css/property/property_id.h"
#include "html/css/property/property_id_enum.h"

namespace html {

class CSSPropertyName {
public:
  CSSPropertyName(CSSPropertyID id) : m_property_id(id) {}
  CSSPropertyName(const AtomicString &custom_name)
      : m_custom_property_name(custom_name), 
        m_property_id(CSSPropertyID::Variable) {}

public:
  CSSPropertyID m_property_id = CSSPropertyID::Invalid;
  AtomicString m_custom_property_name;
};

}
#endif