#ifndef _UI_SDK_HTML_CSS_PROPERTY_VALUEID_H_
#define _UI_SDK_HTML_CSS_PROPERTY_VALUEID_H_

#include "html/css/property/value_id_enum.h"
#include <string>

namespace html {

constexpr size_t MaxCSSValueNameLength = 48;

CSSValueID CSSValueNameToId(const std::u16string& name);

}

#endif