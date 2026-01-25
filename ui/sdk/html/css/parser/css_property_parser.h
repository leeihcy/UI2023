#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPROPERTY_PARSER_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPROPERTY_PARSER_H_

#include "html/css/parser/css_parser.h"
#include "html/css/property/property_id.h"

namespace html {
struct CSSParserContext;
class CSSValue;

class CSSPropertyParser {
public:
  CSSPropertyParser(CSSParserContext &context);

  static bool ParseValue(CSSParserContext &context, CSSPropertyID property_id,
                         bool allow_important);

  bool ParseValueStart(CSSPropertyID property_id, bool allow_important);
  bool ParseCSSWideKeyword(CSSParserContext &context, CSSPropertyID property_id, bool allow_important);

  static A<CSSValue> ConsumeCSSWideKeyword(CSSParserContext &context,
                                           bool allow_important_annotation,
                                           bool &important);

private:
  CSSParserContext &m_context;
};

} // namespace html

#endif