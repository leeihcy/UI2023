#ifndef _HTML_CSS_PROPERTY_CSSPROPERTYINSTANCES_H_
#define _HTML_CSS_PROPERTY_CSSPROPERTYINSTANCES_H_

namespace html {
class CSSProperty;
enum class CSSPropertyId;

const CSSProperty& GetCSSPropertyInternal(CSSPropertyId id);

}

#endif