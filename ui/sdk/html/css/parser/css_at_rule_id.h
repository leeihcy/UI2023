#ifndef _HTML_CSS_PARSER_CSSATRULEID_H_
#define _HTML_CSS_PARSER_CSSATRULEID_H_

#include <string>

namespace html {

enum class CSSAtRuleID {
  Invalid,

  Charset,
  FontFace,
  Import,
  Keyframes,
  Namespace,

  kCSSAtRuleContents,
  kCSSAtRuleMedia,
  kCSSAtRuleSupports,
  kCSSAtRuleContainer,
  kCSSAtRuleNavigation,
  kCSSAtRuleLayer,
  kCSSAtRuleScope,
  kCSSAtRuleStartingStyle,
  kCSSAtRuleViewTransition,
  kCSSAtRuleApplyMixin,

  kCSSAtRuleTopLeftCorner,
  kCSSAtRuleTopLeft,
  kCSSAtRuleTopCenter,
  kCSSAtRuleTopRight,
  kCSSAtRuleTopRightCorner,
  kCSSAtRuleBottomLeftCorner,
  kCSSAtRuleBottomLeft,
  kCSSAtRuleBottomCenter,
  kCSSAtRuleBottomRight,
  kCSSAtRuleBottomRightCorner,
  kCSSAtRuleLeftTop,
  kCSSAtRuleLeftMiddle,
  kCSSAtRuleLeftBottom,
  kCSSAtRuleRightTop,
  kCSSAtRuleRightMiddle,
  kCSSAtRuleRightBottom,
  Count,
};

CSSAtRuleID CssAtRuleID(const std::u16string& name);

// StringView CssAtRuleIDToString(CSSAtRuleID id);
}

#endif