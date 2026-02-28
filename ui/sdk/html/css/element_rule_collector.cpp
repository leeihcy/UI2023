#include "html/css/element_rule_collector.h"


namespace html {

MatchedRule::MatchedRule(const RuleData *rule_data, uint16_t layre_order,
                         unsigned proximity, unsigned style_sheet_index)
    : rule_(rule_data->Rule()) {}

void ElementRuleCollector::DidMatchRule(const RuleData *rule_data,
                                        int style_sheet_index) {
  m_matched_rules.emplace_back(rule_data, 0, 0, style_sheet_index);
}


void ElementRuleCollector::SortAndTransferMatchedRules(
    CascadeOrigin origin,
    bool is_vtt_embedded_style/*,
    StyleRuleUsageTracker* tracker*/) {
  if (m_matched_rules.empty()) {
    return;
  }
  // SortMatchedRules();

  for (const MatchedRule& matched_rule : m_matched_rules) {
    m_result.AddMatchedProperties(
      &matched_rule.Rule()->Properties()
    );
  }

}

}