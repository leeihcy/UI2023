#include "html/css/style_color.h"
#include "html/css/property/color_enum.h"

namespace html {

Color StyleColor::ColorFromKeyword(CSSValueID keyword,
                                   ColorScheme color_scheme) {
  const char* value_name = CSSValueIDString(keyword);
  if (value_name) {
    unsigned int argb_value = ResolveColor(value_name, strlen(value_name));
    return Color::FromRGBA32(argb_value);
  }
  // return LayoutTheme::GetTheme().SystemColor(keyword, color_scheme);
  return Color();
}

}