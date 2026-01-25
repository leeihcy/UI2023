#include "html/css/parser/css_variable_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token_stream.h"

namespace html {


bool CSSVariableParser::IsValidVariableName(const CSSParserToken& token) {
  if (token.GetType() != CSSParserTokenType::Ident) {
    return false;
  }
  return IsValidVariableName(token.Name());
}

bool CSSVariableParser::IsValidVariableName(const std::u16string& string) {
  return string.length() >= 3 && string[0] == '-' && string[1] == '-';
}

// --x:foo;
bool CSSVariableParser::StartsCustomPropertyDeclaration(CSSParserTokenStream& stream) {
   if (!CSSVariableParser::IsValidVariableName(stream.Peek())) {
    return false;
  }
  CSSParserTokenStream::State state = stream.Save();
  stream.ConsumeIncludingWhitespace();  // <ident>
  bool result = stream.Peek().GetType() == CSSParserTokenType::Colon;
  stream.Restore(state);
  return result;
}

}
