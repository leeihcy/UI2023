#ifndef _HTML_CSS_PARSER_CSS_VARIABLE_PARSER_H_
#define _HTML_CSS_PARSER_CSS_VARIABLE_PARSER_H_

#include <string>

namespace html {
class CSSParserTokenStream;
class CSSParserToken;

class CSSVariableParser {
public:
  static bool IsValidVariableName(const CSSParserToken& token);
  static bool IsValidVariableName(const std::u16string& string);

  static bool StartsCustomPropertyDeclaration(CSSParserTokenStream&);

};

}

#endif