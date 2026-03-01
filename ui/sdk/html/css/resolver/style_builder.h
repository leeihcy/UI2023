#ifndef _HTML_CSS_RESOLVER_STYLEBUILDER_H_
#define _HTML_CSS_RESOLVER_STYLEBUILDER_H_

namespace html {
class CSSPropertyName;
class StyleResolverState;
class CSSValue;
class CSSProperty;

class StyleBuilder {
 public:
  //  static void ApplyProperty(const CSSPropertyName &, StyleResolverState &,
  //                            const ScopedCSSValue &);
   static void ApplyProperty(const CSSProperty &, StyleResolverState &,
                             const CSSValue &value);
};

}

#endif