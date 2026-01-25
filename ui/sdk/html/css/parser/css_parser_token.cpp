#include "html/css/parser/css_parser_token.h"

namespace html {

CSSValueID CSSParserToken::ValueId() const {
  if (m_type != CSSParserTokenType::Ident) {
    return CSSValueID::Invalid;
  }

  return CSSValueNameToId(m_name.c_str());
}
CSSValueID CSSParserToken::FunctionId() {
  if (m_type != CSSParserTokenType::Function) {
      return CSSValueID::Invalid;
    }
    return CSSValueNameToId(m_name.c_str());
}

} // namespace html