#ifndef _HTML_CSS_RULE_SET_H_
#define _HTML_CSS_RULE_SET_H_

#include "html/css/css_selector.h"
#include "html/css/style_rule.h"
#include "html/css/style_rule_import.h"
#include "html/css/style_scope.h"

namespace html {


// 将 StyleRule 中的数据，按 selector 维度拆分开来，每项做为一个RuleData。
class RuleData {
public:
  RuleData(StyleRule& style_rule, unsigned selector_index, unsigned position) :
    m_style_rule(style_rule), m_selector_index(selector_index), m_position(position)
  {}

  CSSSelector& MutableSelector() {
    return m_style_rule.GetMutableSelectorAt(m_selector_index);
  }
private:
  StyleRule& m_style_rule;
  unsigned m_selector_index;
  unsigned m_position;
};

class RuleSet {
public:
  void AddRulesFromSheet(const StyleSheetContents* sheet);
  void AddChildRules(StyleRule* parent_rule, const std::vector<A<StyleRuleBase>>& rules);
  void AddStyleRule(StyleRule* style_rule, StyleRule* parent_rule);
  void AddRule(StyleRule* style_rule, size_t selector_index);


  enum class BucketCoverage {
    kIgnore,
    kCompute,
  };
  template<BucketCoverage bucket_coverage>
  void FindBestBucketAndAdd(CSSSelector&, const RuleData&);
  

  // If a single @scope rule covers all rules in this RuleSet,
  // returns the corresponding StyleScope rule, or returns nullptr otherwise.
  //
  // This is useful for rejecting entire RuleSets early when implicit @scopes
  // aren't in scope.
  //
  // See ElementRuleCollector::CanRejectScope.
  const StyleScope* SingleScope() const {
    // if (scope_intervals_.size() == 1u) {
    //   const Interval<StyleScope>& interval = scope_intervals_.front();
    //   if (interval.start_position == 0) {
    //     return interval.value.Get();
    //   }
    // }
    return nullptr;
  }

private:
  unsigned m_rule_count = 0;
};

}

#endif