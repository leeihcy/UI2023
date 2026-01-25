#ifndef _HTML_CSS_PARSER_CSS_VARIABLE_PARSER_H_
#define _HTML_CSS_PARSER_CSS_VARIABLE_PARSER_H_

#include "html/base/memory.h"
#include <string>

namespace html {
class CSSParserTokenStream;
class CSSParserToken;
class CSSVariableData;

class CSSVariableParser {
public:
  static bool IsValidVariableName(const CSSParserToken& token);
  static bool IsValidVariableName(const std::u16string& string);

  static bool StartsCustomPropertyDeclaration(CSSParserTokenStream&);
  
  static A<CSSVariableData> ConsumeUnparsedDeclaration(
      CSSParserTokenStream& stream,
      bool allow_important_annotation,
      bool is_animation_tainted,
      bool must_contain_variable_reference,
      bool restricted_value,
      bool comma_ends_declaration,
      bool& important/*,
      const CSSParserContext& context*/);
};

}

#endif