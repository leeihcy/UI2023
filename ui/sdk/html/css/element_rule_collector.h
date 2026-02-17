#ifndef _HTML_CSS_ELEMENT_RULE_COLLECTOR_H_
#define _HTML_CSS_ELEMENT_RULE_COLLECTOR_H_

#include <cassert>
#include <vector>
#include "html/css/resolver/match_request.h"
#include "html/css/resolver/cascade_origin.h"
#include "html/dom/element.h"

namespace html {

 
class MatchedRule {
  
};
class PartNames {

};
class StyleRuleUsageTracker {

};

class ElementRuleCollector {
public:
  void ClearMatchedRules() {
    m_matched_rules.clear();
  }
  void SetElement(Element* element) {
    m_element = element;
  }
  void CollectMatchingRules(const MatchRequest &match_request,
                            PartNames *part_names) {
    CollectMatchingRulesInternal</*stop_at_first_match=*/false>(match_request,
                                                                part_names);
  }


// Contains (constructs) a SelectorCheckerContext and some precalculated values
// (e.g. which LayerMap is in use) so that we don't need to do that over and
// over again for each list of selectors we are trying to match.
// struct ContextWithStyleScopeFrame {
//  public:
//   ContextWithStyleScopeFrame(const ElementResolveContext& element_context,
//                              const MatchRequest& match_request,
//                              StyleRequest* pseudo_style_request,
//                              StyleScopeFrame* parent_frame,
//                              bool matching_ua_rules,
//                              bool matching_rules_from_no_style_sheet,
//                              const StyleRecalcContext& style_recalc_context,
//                              SelectorChecker::Mode mode) {

//                              }
//   }

  template <bool stop_at_first_match>
  bool CollectMatchingRulesInternal(const MatchRequest &match_request,
                                    PartNames *part_names) {

  // ContextWithStyleScopeFrame context(
  //     context_, match_request, &pseudo_style_request_,
  //     style_recalc_context_.style_scope_frame, matching_ua_rules_,
  //     matching_rules_from_no_style_sheet_, style_recalc_context_, mode_);

  // Element& element = *context.context.element;
  Element& element = *m_element;
    
  // We need to collect the rules for id, class, tag, and everything else into a
  // buffer and then sort the buffer.
  if (element.HasID()) {
    for (const auto bundle : match_request.AllRuleSets()) {
    //   if (CollectMatchingRulesForList<stop_at_first_match>(
    //           bundle.rule_set->IdRules(element.IdForStyleResolution()),
    //           match_request, bundle.rule_set, bundle.style_sheet_index, checker,
    //           context) &&
    //       stop_at_first_match) {
    //     return true;
    //   }
    }
  }

    return false;
  }

void SortAndTransferMatchedRules(
    CascadeOrigin origin,
    bool is_vtt_embedded_style/*,
    StyleRuleUsageTracker* tracker*/) {
}

private:
  Element* m_element = nullptr;
  std::vector<MatchedRule> m_matched_rules;
};
}


#endif