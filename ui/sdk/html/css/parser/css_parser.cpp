#include "html/css/parser/css_parser.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/util/util.h"
#include <string>

namespace html {

// static
void CSSParser::ParseInlineStyleDeclaration(const char* bytes, size_t length) {
  std::u16string unicode;
  html::Utf8ToUnicode(bytes, length, unicode);
}

}