#ifndef _HTML_CSS_PROPERTY_CSSPROPERTYINSTANCES_H_
#define _HTML_CSS_PROPERTY_CSSPROPERTYINSTANCES_H_

namespace html {
class CSSProperty;
enum class CSSPropertyID;

const CSSProperty& GetCSSPropertyInternal(CSSPropertyID id);

}

#endif