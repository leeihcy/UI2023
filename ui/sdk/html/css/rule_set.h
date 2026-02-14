#ifndef _HTML_CSS_RULE_SET_H_
#define _HTML_CSS_RULE_SET_H_

#include "html/css/css_selector.h"
#include "html/css/style_rule.h"
#include "html/css/style_rule_import.h"
#include <cstddef>
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
  
private:
  unsigned m_rule_count = 0;
};

}

#endif