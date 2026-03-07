#ifndef _HTML_CSS_RESOLVER_STYLE_CASCADE_H_
#define _HTML_CSS_RESOLVER_STYLE_CASCADE_H_
#include "html/css/resolver/match_result.h"
#include "html/css/resolver/style_resolver_state.h"
#include "html/css/resolver/cascade_filter.h"
#include "html/css/resolver/cascade_priority.h"

namespace html {
class CascadeResolver;
class CSSPropertyValue;

class StyleCascade {
public:
  StyleCascade(StyleResolverState& state) : m_state(state) {}
  MatchResult& MutableMatchResult() { return m_match_result; }
  const MatchResult& GetMatchResult() { return m_match_result; }

  void Apply(CascadeFilter filter);
  void ApplyMatchResult(CascadeResolver& resolver);
  void LookupAndApplyDeclaration(CSSPropertyValue&,
                                 CascadePriority*,
                                 CascadeResolver&);
private:
  StyleResolverState m_state;
  MatchResult m_match_result;

  uint8_t generation_ = 0;
};

}

#endif
