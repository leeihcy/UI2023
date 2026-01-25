#include "html/css/style_property_shorthand.h"
#include "html/css/property/property_id.h"


namespace html {


const StylePropertyShorthand& backgroundShorthand() {
  // 注：Background 不包含 blend-mode

  static const CSSProperty* longhands[] = {
    &CSSProperty::Get(CSSPropertyID::BackgroundAttachment),
    &CSSProperty::Get(CSSPropertyID::BackgroundClip),
    &CSSProperty::Get(CSSPropertyID::BackgroundColor),
    &CSSProperty::Get(CSSPropertyID::BackgroundImage),
    &CSSProperty::Get(CSSPropertyID::BackgroundOrigin),
    &CSSProperty::Get(CSSPropertyID::BackgroundPositionX),
    &CSSProperty::Get(CSSPropertyID::BackgroundPositionY),
    &CSSProperty::Get(CSSPropertyID::BackgroundRepeat),
    &CSSProperty::Get(CSSPropertyID::BackgroundSize)
  };

  static const StylePropertyShorthand shorthand(CSSPropertyID::Background,
                                                (const CSSProperty **)longhands,
                                                std::size(longhands));

  return shorthand;
}

}