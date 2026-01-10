#include "html/css/style_property_shorthand.h"
#include "html/css/property/property_id.h"


namespace html {


const StylePropertyShorthand& backgroundShorthand() {
  // 注：Background 不包含 blend-mode

  static const CSSProperty* longhands[] = {
    &CSSProperty::Get(CSSPropertyId::BackgroundAttachment),
    &CSSProperty::Get(CSSPropertyId::BackgroundClip),
    &CSSProperty::Get(CSSPropertyId::BackgroundColor),
    &CSSProperty::Get(CSSPropertyId::BackgroundImage),
    &CSSProperty::Get(CSSPropertyId::BackgroundOrigin),
    &CSSProperty::Get(CSSPropertyId::BackgroundPositionX),
    &CSSProperty::Get(CSSPropertyId::BackgroundPositionY),
    &CSSProperty::Get(CSSPropertyId::BackgroundRepeat),
    &CSSProperty::Get(CSSPropertyId::BackgroundSize)
  };

  static const StylePropertyShorthand shorthand(CSSPropertyId::Background,
                                                (const CSSProperty **)longhands,
                                                std::size(longhands));

  return shorthand;
}

}