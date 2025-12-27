#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_

#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/property/property_id.h"

#include <string>
#include <vector>


namespace html {

class CSSPropertyValue {

};

struct CSSParserContext {
  CSSParserTokenStream token_stream;
  std::vector<CSSPropertyValue> parsed_properties;
};

class CSSParser {
public:
  void ParseInlineStyleDeclaration(const char* bytes, size_t size);

protected:
  void ConsumeBlockContents(CSSParserContext& context);
  bool ConsumeDeclaration(CSSParserContext& context);
  void ConsumeDeclarationValue(CSSParserContext& context, CSSPropertyId property_id);
};

}

#endif