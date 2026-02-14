#ifndef _HTML_CSS_RESOLVER_STYLERESOLVER_H_
#define _HTML_CSS_RESOLVER_STYLERESOLVER_H_

namespace html {
class ComputedStyle;
class ElementRuleCollector;
class Element;
class StyleResolverState;

class StyleResolver {
public:
 enum UASheetCacheKeyIndex {
    kHTMLUASheet,
    kSVGUASheet,
    kMathMLUASheet,
    kFullscreenUASheet,
    kPrintUASheet,
    kQuirksUASheet,
    kViewSourceUASheet,
    kForcedColorsUASheet,
    kJSONUASheet,
    kViewTransitionUASheet,
    kPseudoElementUASheet,
  };
  
  const ComputedStyle* ResolveStyle();
  void ApplyBaseStyle();
  void ApplyBaseStyleNoCache(
    Element *element,
    // const StyleRecalcContext& style_recalc_context,
    // const StyleRequest& style_request,
    StyleResolverState &state /*,
     StyleCascade& cascade*/
  );

  void MatchAllRules(StyleResolverState& state, ElementRuleCollector& collector);
  void MatchUARules(const Element& element, ElementRuleCollector& collector);

  template <typename Functor>
  void ForEachUARulesForElement(const Element& element,
                                ElementRuleCollector* collector,
                                Functor& func) const;


private:
  bool m_print_media_type = false;
};

}

#endif