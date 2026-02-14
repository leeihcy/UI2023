#include "html/css/resolver/style_resolver.h"
#include "html/css/element_rule_collector.h"
#include "html/css/resolver/style_resolver.h"
#include "html/css/resolver/style_resolver_state.h"
#include "html/css/rule_set.h"
#include "html/css/resolver/match_request.h"
#include "html/css/css_default_style_sheets.h"


namespace html {

//
// third_party\blink\renderer\core\css\resolver\style_resolver.cc
//
const ComputedStyle* StyleResolver::ResolveStyle() {

  // StyleResolverState state();
  // StyleCascade cascade(state);

  return nullptr;
}

void StyleResolver::ApplyBaseStyle() {
  // ApplyBaseStyleNoCache();
}

void StyleResolver::ApplyBaseStyleNoCache(
    Element *element,
    // const StyleRecalcContext& style_recalc_context,
    // const StyleRequest& style_request,
    StyleResolverState &state /*,
     StyleCascade& cascade*/
) {
  ElementRuleCollector collector;
  // StyleResolverState state;
  MatchAllRules(state, collector);
}

void StyleResolver::MatchAllRules(
  StyleResolverState& state,
  ElementRuleCollector& collector/*,
  bool include_smil_properties*/
) {
  Element& element = state.GetElement();

  MatchUARules(element, collector);
  // MatchUserRules(collector);
  // MatchPresentationalHints(collector);
  // MatchAuthorRules(collector);
}

void StyleResolver::MatchUARules(const Element &element,
                                 ElementRuleCollector &collector) {
  // collector.SetMatchingUARules(true);

  RuleSetGroup rule_set_group(/*rule_set_group_index=*/0u);
  auto func2 = [&rule_set_group](RuleSet *rules, unsigned rule_set_index) {
    rule_set_group.AddRuleSet(rules);
  };
  ForEachUARulesForElement(element, &collector, func2);
}

template <typename Functor>
void StyleResolver::ForEachUARulesForElement(const Element &element,
                                             ElementRuleCollector *collector,
                                             Functor &func) const {
  CSSDefaultStyleSheets &default_style_sheets =
      CSSDefaultStyleSheets::GetInstance();
  if (!m_print_media_type) {
    if (element.IsHTMLElement()/* || element.IsPseudoElement() ||
        element.IsVTTElement()*/) {
      func(default_style_sheets.DefaultHtmlStyle(), kHTMLUASheet);
    }
    // ... svg, math 
    // full screen...
  } else {
    // func(default_style_sheets.DefaultPaintStyle(), kPrintUASheet);
  }
  // Quirks mode

  // View source mode

  // ... others
}

}