#ifndef _HTML_CSS_RESOLVER_STYLERESOLVER_H_
#define _HTML_CSS_RESOLVER_STYLERESOLVER_H_

namespace html {
class ComputedStyle;

class StyleResolver {
public:
  const ComputedStyle* ResolveStyle();
  void ApplyBaseStyle();
};

}

#endif