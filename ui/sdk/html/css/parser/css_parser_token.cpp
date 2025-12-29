#include "html/css/parser/css_parser_token.h"

namespace html {

CSSValueId CSSParserToken::ValueId() {
  if (m_type != CSSParserTokenType::Ident) {
    return CSSValueId::Invalid;
  }

  return CSSValueNameToId(m_name.c_str());
}
CSSValueId CSSParserToken::FunctionId() {
  if (m_type != CSSParserTokenType::Function) {
      return CSSValueId::Invalid;
    }
    return CSSValueNameToId(m_name.c_str());
}

} // namespace html