#ifndef _HTML_CSS_STYLERULE_NAMESPACE_H_
#define _HTML_CSS_STYLERULE_NAMESPACE_H_

#include "html/css/style_rule.h"
#include "html/base/atomic_string.h"

namespace html {

class StyleRuleNamespace final : public StyleRuleBase {
 public:
  StyleRuleNamespace(AtomicString prefix, AtomicString uri)
      : StyleRuleBase(StyleRuleBase::kNamespace), m_prefix(prefix), m_uri(uri) {}

  const AtomicString& Prefix() { return m_prefix; }
  const AtomicString& Uri() { return m_uri; }
private:
  AtomicString m_prefix;
  AtomicString m_uri;
};

template<>
struct DowncastTraits<StyleRuleNamespace> {
  static bool AllowFrom(const StyleRuleBase& from) {
    return from.GetType() == StyleRuleBase::kNamespace;
  }
};

}

#endif