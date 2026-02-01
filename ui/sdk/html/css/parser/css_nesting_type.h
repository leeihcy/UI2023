#ifndef _HTML_CSS_PARSER_CSSNESTINGTYPE_H_
#define _HTML_CSS_PARSER_CSSNESTINGTYPE_H_

namespace html {

enum class CSSNestingType {
  None,
  Scope,
  Nesting,
  Function,
};

}
#endif