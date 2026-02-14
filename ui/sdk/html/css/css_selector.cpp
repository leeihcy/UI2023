#include "html/css/css_selector.h"
#include "html/css/parser/css_selector_parser.h"

namespace html {

CSSSelector::CSSSelector(MatchType match_type, const QualifiedName &attribute,
                         AttributeMatchType case_sensitivity)
    : m_match_type(match_type), m_tag_q_name_or_attribute(attribute),
      m_case_sensitivity(case_sensitivity) {
  OnConstruct();
}

CSSSelector::CSSSelector(MatchType match_type, const QualifiedName &attribute,
                         AttributeMatchType case_sensitivity,
                         const AtomicString &value)
    : m_match_type(match_type), m_tag_q_name_or_attribute(attribute),
      m_case_sensitivity(case_sensitivity), m_value(value) {
  OnConstruct();
}

void CSSSelector::OnConstruct() {
  int a = 0;
}

void CSSSelector::UpdatePseudoType(const AtomicString &value,
                        /*const CSSParserContext &context, */
                        bool has_arguments/*,
                        CSSParserMode mode*/)
{
  assert(GetMatchType() == PseudoClass || GetMatchType() == PseudoElement);
  AtomicString lower_value = value.Lower();
  PseudoType pseudo_type = CSSSelectorParser::ParsePseudoType(
      lower_value, has_arguments/*, context.GetDocument()*/);
  SetPseudoType(pseudo_type);
  SetValue(lower_value);

  // 堆栈检查。
  // switch (GetPseudoType()) {
  // }

}

CSSSelector::PseudoType CSSSelector::NameToPseudoType(
    const std::u16string& name,
    bool has_arguments/*,
    const Document* document*/) {
  // assert(false && "todo: perfect hash");
  
  static std::map<std::u16string, CSSSelector::PseudoType> pseudo_map = {
    { u"hover", CSSSelector::kPseudoHover },
    { u"focus", CSSSelector::kPseudoFocus },
    { u"checked", CSSSelector::kPseudoChecked },
    { u"active", CSSSelector::kPseudoActive },
    { u"has", CSSSelector::kPseudoHas },
    { u"after", CSSSelector::kPseudoAfter },
    { u"before", CSSSelector::kPseudoBefore },
    { u"root", CSSSelector::kPseudoRoot},
    { u"scope", CSSSelector::kPseudoScope},
  };
  auto iter = pseudo_map.find(name);
  if (iter != pseudo_map.end()) {
    return iter->second;
  }

  return CSSSelector::PseudoType::kPseudoUnknown;
}

CSSNestingType CSSSelector::GetNestingType() const {
    switch (GetPseudoType()) {
    case CSSSelector::kPseudoParent:
      return CSSNestingType::Nesting;
    case CSSSelector::kPseudoUnparsed:
      return m_nesting_type;
    case CSSSelector::kPseudoScope:
      return CSSNestingType::Scope;
    default:
      return CSSNestingType::None;
  }
}

} // namespace html
