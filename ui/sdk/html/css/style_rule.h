#ifndef _HTML_CSS_STYLERULE_H_
#define _HTML_CSS_STYLERULE_H_

namespace html {

class StyleRule {
public:
  enum RuleType {
    kStyle,
    kFontFace,
    kKeyframes,
    kKeyframe,
  };
  RuleType m_type;
};

}

#endif