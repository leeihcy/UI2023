#include "html/css/css_selector.h"

namespace html {

CSSSelector::CSSSelector(MatchType match_type, const QualifiedName &attribute,
                         AttributeMatchType case_sensitivity)
    : m_match_type(match_type), m_tag_q_name_or_attribute(attribute),
      m_case_sensitivity(case_sensitivity) {}

CSSSelector::CSSSelector(MatchType match_type, const QualifiedName &attribute,
                         AttributeMatchType case_sensitivity,
                         const AtomicString &value)
    : m_match_type(match_type), m_tag_q_name_or_attribute(attribute),
      m_case_sensitivity(case_sensitivity), m_value(value) {}

} // namespace html
