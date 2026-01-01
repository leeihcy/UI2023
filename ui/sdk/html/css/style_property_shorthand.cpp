#include "html/css/style_property_shorthand.h"
#include "html/css/property/property_id.h"


namespace html {


const StylePropertyShorthand& backgroundShorthand() {
  static const CSSProperty* longhands[] = {
#if 0
    &GetCSSPropertyBackgroundImage(),
    &GetCSSPropertyBackgroundPositionX(),
    &GetCSSPropertyBackgroundPositionY(),
    &GetCSSPropertyBackgroundSize(),
    &GetCSSPropertyBackgroundRepeat(),
    &GetCSSPropertyBackgroundAttachment(),
    &GetCSSPropertyBackgroundOrigin(),
    &GetCSSPropertyBackgroundClip(),
    &GetCSSPropertyBackgroundColor(),
#endif
    &CSSProperty::Get(CSSPropertyId::BackgroundColor)
  };

  static const StylePropertyShorthand shorthand(CSSPropertyId::Background,
                                                (const CSSProperty *)longhands,
                                                std::size(longhands));
  return shorthand;
}

}