#include "html/css/resolver/style_resolver.h"
#include "html/css/element_rule_collector.h"
#include "html/css/resolver/style_resolver.h"
#include "html/css/resolver/style_resolver_state.h"
#include "html/css/rule_set.h"
#include "html/css/resolver/match_request.h"
#include "html/css/style_request.h"
#include "html/css/css_default_style_sheets.h"
#include "html/css/resolver/style_cascade.h"

namespace html {

//
// third_party\blink\renderer\core\css\resolver\style_resolver.cc
//
const ComputedStyle *
StyleResolver::ResolveStyle(Element *element,
                            const StyleRecalcContext &style_recalc_context,
                            const StyleRequest &style_request) {
  if (!element) {
    return nullptr;
  }

  StyleResolverState state(
      GetDocument(), *element, &style_recalc_context, style_request);

  StyleCascade cascade(state);

  ApplyBaseStyle(element, style_recalc_context, style_request, state, cascade);
  return nullptr;
}

void StyleResolver::ApplyBaseStyle(
    Element *element, const StyleRecalcContext &style_recalc_context,
    const StyleRequest &style_request, StyleResolverState &state,
    StyleCascade &cascade) {
  ApplyBaseStyleNoCache(element, style_recalc_context, style_request, state,
                        cascade);
}

void StyleResolver::ApplyBaseStyleNoCache(
    Element *element, const StyleRecalcContext &style_recalc_context,
    const StyleRequest &style_request, StyleResolverState &state,
    StyleCascade &cascade) {
  ElementRuleCollector collector;
  collector.SetElement(element);
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

  if (!rule_set_group.IsEmpty()) {
    collector.ClearMatchedRules();
    MatchRequest match_request(rule_set_group, /*scope=*/nullptr);
    collector.CollectMatchingRules(match_request, /*part_names*/ nullptr);
    collector.SortAndTransferMatchedRules(
        CascadeOrigin::kUserAgent, /*is_vtt_embedded_style=*/false/*, tracker_*/);

  }

  // if (IsInMediaUAShadow(element)) {
  //   //
  // }

  // collector.SetMatchingUARules(false);
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