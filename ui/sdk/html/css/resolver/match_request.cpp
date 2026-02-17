#include "html/css/resolver/match_request.h"
#include <assert.h>
#include "html/css/rule_set.h"

namespace html {

void RuleSetGroup::AddRuleSet(const RuleSet* rule_set) {
  size_t num_rule_sets = m_rule_sets.size();

  m_rule_sets.push_back(rule_set);
  // ++ m_num_rule_sets;

  const RuleSetBitmap rule_set_mask = RuleSetBitmap{1} << num_rule_sets;
  if (rule_set->SingleScope()) {
    single_scope_ |= rule_set_mask;
  } else {
    not_single_scope_ |= rule_set_mask;
  }

}

}