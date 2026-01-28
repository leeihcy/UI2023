#ifndef _UI_SDK_HTML_CSS_PROPERTY_PROPERTYID_EXT_H_
#define _UI_SDK_HTML_CSS_PROPERTY_PROPERTYID_EXT_H_

#include "html/css/property/property_id_enum.h"

#include <string>
namespace html {

constexpr size_t MaxCSSPropertyNameLength = 48;

CSSPropertyID CSSPropertyNameToId(const std::u16string& name);

}

#endif