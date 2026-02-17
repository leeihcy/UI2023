#ifndef _HTML_CSS_RESOLVER_STYLERESOLVER_H_
#define _HTML_CSS_RESOLVER_STYLERESOLVER_H_

#include "html/css/style_request.h"
#include "html/css/style_recalc_context.h"

namespace html {
class ComputedStyle;
class ElementRuleCollector;
class Element;
class Document;
class StyleResolverState;
class StyleCascade;

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
  
  Document& GetDocument() const { return *m_document; }

  const ComputedStyle *
  ResolveStyle(Element *element, const StyleRecalcContext &style_recalc_context,
               const StyleRequest &style_request);
  void ApplyBaseStyle(Element* element,
                      const StyleRecalcContext&,
                      const StyleRequest&,
                      StyleResolverState& state,
                      StyleCascade& cascade);
  void ApplyBaseStyleNoCache(
    Element *element,
    const StyleRecalcContext& style_recalc_context,
    const StyleRequest& style_request,
    StyleResolverState &state,
     StyleCascade& cascade
  );

  void MatchAllRules(StyleResolverState& state, ElementRuleCollector& collector);
  void MatchUARules(const Element& element, ElementRuleCollector& collector);

  template <typename Functor>
  void ForEachUARulesForElement(const Element& element,
                                ElementRuleCollector* collector,
                                Functor& func) const;


private:
  Document* m_document = nullptr;
  bool m_print_media_type = false;
};

}

#endif