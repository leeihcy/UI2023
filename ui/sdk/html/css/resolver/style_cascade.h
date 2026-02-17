#ifndef _HTML_CSS_RESOLVER_STYLE_CASCADE_H_
#define _HTML_CSS_RESOLVER_STYLE_CASCADE_H_
#include "html/css/resolver/style_resolver_state.h"

namespace html {

class StyleCascade {
public:
   StyleCascade(StyleResolverState& state) : m_state(state) {}

private:
  StyleResolverState m_state;
};

}

#endif
