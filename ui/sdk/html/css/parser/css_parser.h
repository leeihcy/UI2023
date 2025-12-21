#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_

#include <string>

namespace html {

class CSSParser {
public:
  static void ParseInlineStyleDeclaration(const char* bytes, size_t length);
};

}

#endif