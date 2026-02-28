#ifndef _HTML_CSS_RESOLVER_STYLE_CASCADE_H_
#define _HTML_CSS_RESOLVER_STYLE_CASCADE_H_
#include "html/css/resolver/match_result.h"
#include "html/css/resolver/style_resolver_state.h"

namespace html {

class StyleCascade {
public:
  StyleCascade(StyleResolverState& state) : m_state(state) {}
  MatchResult& MutableMatchResult() { return m_match_result; }

private:
  StyleResolverState m_state;
  MatchResult m_match_result;
};

}

#endif
