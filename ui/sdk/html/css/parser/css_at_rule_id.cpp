#include "html/css/parser/css_at_rule_id.h"
#include "html/util/util.h"


namespace html {

CSSAtRuleID CssAtRuleID(const std::u16string& name) {
  if (EqualIgnoringASCIICase(name.c_str(), u"namespace")) {
    return CSSAtRuleID::Namespace;
  }

  return CSSAtRuleID::Invalid;
}

}