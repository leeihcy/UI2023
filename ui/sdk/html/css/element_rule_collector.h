#ifndef _HTML_CSS_ELEMENT_RULE_COLLECTOR_H_
#define _HTML_CSS_ELEMENT_RULE_COLLECTOR_H_

#include <cassert>
#include <vector>
#include <span>
#include "html/css/selector_checker.h"
#include "html/css/rule_set.h"
#include "html/css/resolver/match_request.h"
#include "html/css/resolver/match_result.h"
#include "html/css/resolver/cascade_origin.h"
#include "html/dom/element.h"

namespace html {

class MatchedRule {
public:
  MatchedRule(const RuleData *rule_data, uint16_t layre_order,
              unsigned proximity, unsigned style_sheet_index);

  StyleRule* Rule() const { return rule_; }

private:
  StyleRule* rule_ = nullptr;;
};

class PartNames {

};
class StyleRuleUsageTracker {

};

class ElementRuleCollector {
public:
  ElementRuleCollector(MatchResult& result) : m_result(result) {}

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
struct ContextWithStyleScopeFrame {
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
  };


  template <bool stop_at_first_match>
  bool /*ElementRuleCollector::*/CollectMatchingRulesForList(
      std::span<const RuleData> rules,
      const MatchRequest& match_request,
      const RuleSet* rule_set,
      int style_sheet_index,
      const SelectorChecker& checker,
      ContextWithStyleScopeFrame& context) {
    // This is a very common case for many style sheets, and by putting it here
    // instead of inside CollectMatchingRulesForListInternal(), we're usually
    // inlined into the caller (which saves on stack setup and call overhead
    // in that common case).
    if (rules.empty()) {
      return false;
    }

    // To reduce branching overhead for the common case, we use a template
    // parameter to eliminate branching in CollectMatchingRulesForListInternal
    // when tracing is not enabled.
    // if (!SelectorStatisticsFlag::IsEnabled()) {
    //   return CollectMatchingRulesForListInternal<stop_at_first_match, false>(
    //       rules, match_request, rule_set, style_sheet_index, checker, context);
    // } else {
      return CollectMatchingRulesForListInternal<stop_at_first_match, true>(
          rules, match_request, rule_set, style_sheet_index, checker, context);
    // }
  }


  template <bool stop_at_first_match, bool perf_trace_enabled>
  bool CollectMatchingRulesForListInternal(
    std::span<const RuleData> rules,
    const MatchRequest& match_request,
    const RuleSet* rule_set,
    int style_sheet_index,
    const SelectorChecker& checker,
    ContextWithStyleScopeFrame& context) {
    
    for (const RuleData& rule_data : rules) {
       const auto& selector = rule_data.Selector();
       
      // SelectorChecker::MatchResult result;
      if (rule_data.SelectorIsEasy()) {
        bool easy_match = EasySelectorChecker::Match(&selector, m_element);
        if (!easy_match) {
          continue;
        }
      } else {
        assert(false && "TODO:");
      }
      if (stop_at_first_match) {
        return true;
      }

      unsigned layer_order = 0; //layer_seeker.SeekLayerOrder(rule_data.GetPosition);
      DidMatchRule(&rule_data, style_sheet_index);
    }

    return false;
  }

  void DidMatchRule(const RuleData* rule_data, int style_sheet_index);

  template <bool stop_at_first_match>
  bool CollectMatchingRulesInternal(const MatchRequest &match_request,
                                    PartNames *part_names) {

  SelectorChecker checker/*(part_names, pseudo_style_request_, mode_,
                          matching_ua_rules_)*/;

  ContextWithStyleScopeFrame context;//(
  //     context_, match_request, &pseudo_style_request_,
  //     style_recalc_context_.style_scope_frame, matching_ua_rules_,
  //     matching_rules_from_no_style_sheet_, style_recalc_context_, mode_);

  // Element& element = *context.context.element;
  Element& element = *m_element;
    
  // We need to collect the rules for id, class, tag, and everything else into a
  // buffer and then sort the buffer.
  if (element.HasID()) {
    for (const auto bundle : match_request.AllRuleSets()) {
      if (CollectMatchingRulesForList<stop_at_first_match>(
              bundle.rule_set->IdRules(element.IdForStyleResolution()),
              match_request, bundle.rule_set, bundle.style_sheet_index, checker,
              context) &&
          stop_at_first_match) {
        return true;
      }
    }
  }

    return false;
  }

  void SortAndTransferMatchedRules(
    CascadeOrigin origin,
    bool is_vtt_embedded_style/*,
    StyleRuleUsageTracker* tracker*/);

  const MatchResult& MatchedResult() {
    return m_result;
  }
private:
  Element* m_element = nullptr;
  std::vector<MatchedRule> m_matched_rules;

  MatchResult& m_result;
};
}

#endif