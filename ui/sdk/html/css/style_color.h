#ifndef _HTML_CSS_STYLECOLOR_H_
#define _HTML_CSS_STYLECOLOR_H_

#include "html/css/property/value_id.h"
#include "html/platform/graphics/color.h"
#include "html/dom/text_link_colors.h"

namespace html {

// .button {
//   color: #ff6b6b;
//   border: 2px solid currentColor;  <-- 边框使用 #ff6b6b
// }

class StyleColor {
public:
  StyleColor() = default;
  explicit StyleColor(Color color)
      : color_(color), color_keyword_(CSSValueID::Invalid) {}
  explicit StyleColor(CSSValueID keyword) : color_keyword_(keyword) {}

  static StyleColor GetCurrentColor() { return StyleColor(); }

    inline bool operator==(const StyleColor& other) const {
    // DCHECK(IsValid());
    // DCHECK(other.IsValid());
    return color_ == other.color_ && color_keyword_ == other.color_keyword_;
  }

  inline bool operator!=(const StyleColor& other) const {
    return !(*this == other);
  }
  static Color ColorFromKeyword(CSSValueID,
                                ColorScheme color_scheme);
private:
  Color color_;
  CSSValueID color_keyword_ = CSSValueID::CurrentColor;

};

}

#endif