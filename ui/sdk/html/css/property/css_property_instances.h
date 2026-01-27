#ifndef _HTML_CSS_PROPERTY_CSSPROPERTYINSTANCES_H_
#define _HTML_CSS_PROPERTY_CSSPROPERTYINSTANCES_H_
#include "html/css/property/property_id.h"

namespace html {
class CSSProperty;

const CSSProperty& GetCSSPropertyInternal(CSSPropertyID id);

}

#endif