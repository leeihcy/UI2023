#include "html/dom/text_link_colors.h"
#include "html/css/property/value_id.h"
#include "html/css/property/css_value.h"
#include "html/css/style_color.h"
#include <assert.h>

namespace html {

TextLinkColors::TextLinkColors() : text_color_(Color::kBlack) {
  ResetLinkColor();
  ResetVisitedLinkColor();
  ResetActiveLinkColor();
}

void TextLinkColors::SetTextColor(const Color& color) {
  text_color_ = color;
  has_custom_text_color_ = true;
}

Color TextLinkColors::TextColor() const {
  return TextColor(ColorScheme::kLight);
}

Color TextLinkColors::TextColor(ColorScheme color_scheme) const {
  return has_custom_text_color_
             ? text_color_
             : color_scheme == ColorScheme::kLight
                   ? Color::kBlack
                   : Color::kWhite;
}


Color TextLinkColors::ColorFromCSSValue(const CSSValue& value,
                                        Color current_color,
                                        ColorScheme color_scheme,
                                        bool for_visited_link) const {
  
  // if (auto* color_value = DynamicTo<cssvalue::CSSColor>(value))
  //   return color_value->Value();
  // 
  // if (auto* pair = DynamicTo<CSSLightDarkValuePair>(value)) {
  //   const CSSValue& color_value =
  //       color_scheme == ColorScheme::kLight ? pair->First()
  //                                                         : pair->Second();
  //   return ColorFromCSSValue(color_value, current_color, color_scheme,
  //                            for_visited_link);
  // }

  CSSValueID value_id = To<CSSIdentifierValue>(value).GetValueID();
  switch (value_id) {
    case CSSValueID::Invalid:
      //NOTREACHED();
      abort();
      return Color();
    // case CSSValueID::kInternalQuirkInherit:
    //   return TextColor(color_scheme);
    // case CSSValueID::kWebkitLink:
    //   return for_visited_link ? VisitedLinkColor(color_scheme)
    //                           : LinkColor(color_scheme);
    // case CSSValueID::kWebkitActivelink:
    //   return ActiveLinkColor(color_scheme);
    // case CSSValueID::kWebkitFocusRingColor:
    //   return LayoutTheme::GetTheme().FocusRingColor(color_scheme);
    // case CSSValueID::kCurrentcolor:
    //   return current_color;
    default:
      return StyleColor::ColorFromKeyword(value_id, color_scheme);
  }
}

}