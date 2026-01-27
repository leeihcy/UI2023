#include "html/css/parser/css_parser_token.h"
#include "html/css/property/property_id.h"

namespace html {

CSSValueID CSSParserToken::ValueId() const {
  if (m_type != CSSParserTokenType::Ident) {
    return CSSValueID::Invalid;
  }

  return CSSValueNameToId(m_name.c_str());
}
CSSValueID CSSParserToken::FunctionId() const {
  if (m_type != CSSParserTokenType::Function) {
      return CSSValueID::Invalid;
    }
    return CSSValueNameToId(m_name.c_str());
}

CSSPropertyID CSSParserToken::ParseAsUnresolvedCSSPropertyID() const {
  assert(m_type == CSSParserTokenType::Ident);
  //return UnresolvedCSSPropertyID(execution_context, Value(), mode);
  return CSSPropertyNameToId(Name());
}

AtRuleDescriptorID CSSParserToken::ParseAsAtRuleDescriptorID() const {
  assert(m_type == CSSParserTokenType::Ident);
  return AtRuleDescriptorNameToId(Name());
}

} // namespace html