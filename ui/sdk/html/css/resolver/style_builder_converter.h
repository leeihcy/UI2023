#ifndef _HTML_CSS_RESOLVER_STYLE_BUILDER_CONVERTER_H_
#define _HTML_CSS_RESOLVER_STYLE_BUILDER_CONVERTER_H_

#include "html/css/style_color.h"

namespace html {
class StyleResolverState;
class CSSValue;

class StyleBuilderConverter {
public:
  static StyleColor ConvertStyleColor(
      StyleResolverState &state, const CSSValue &value, bool for_visited_link);
};

}


#endif