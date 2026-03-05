#ifndef _HTML_DOM_TEXT_LINK_COLORS_H_
#define _HTML_DOM_TEXT_LINK_COLORS_H_

#include "html/platform/graphics/color.h"

namespace html {
class CSSValue;

enum class ColorScheme : int32_t {

  kLight = 0,

  kDark = 1,
  kMinValue = 0,
  kMaxValue = 1,
};

class TextLinkColors {
public:
  TextLinkColors();
  TextLinkColors(const TextLinkColors &) = delete;
  TextLinkColors &operator=(const TextLinkColors &) = delete;

  void SetTextColor(const Color &color);
  Color TextColor() const;
  Color TextColor(ColorScheme color_scheme) const;

  void ResetLinkColor() { has_custom_link_color_ = false; }
  void ResetVisitedLinkColor() { has_custom_visited_link_color_ = false; }
  void ResetActiveLinkColor() { has_custom_active_link_color_ = false; }

  Color ColorFromCSSValue(const CSSValue &, Color current_color,
                          ColorScheme color_scheme,
                          bool for_visited_link = false) const;

private:
  Color text_color_;
  Color link_color_;
  Color visited_link_color_;
  Color active_link_color_;

  bool has_custom_text_color_{false};
  bool has_custom_link_color_{false};
  bool has_custom_visited_link_color_{false};
  bool has_custom_active_link_color_{false};
};

} // namespace html

#endif