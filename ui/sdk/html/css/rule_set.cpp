#include "html/css/rule_set.h"
#include "html/base/atomic_string.h"
#include "html/base/casting.h"
#include "html/css/css_selector.h"
#include "html/css/style_rule.h"
#include "html/css/style_sheet_contents.h"
#include <cassert>

namespace html {

namespace {

struct BucketingValues {
public:
  AtomicString id;
  AtomicString class_name;
  AtomicString attr_name;
  AtomicString attr_value;
  bool is_exact_attr = false;
  AtomicString custom_pseudo_element_name;
  AtomicString tag_name;
  AtomicString part_name;
  AtomicString ua_shadow_pseudo;
  CSSSelector::PseudoType pseudo_type = CSSSelector::PseudoType::kPseudoUnknown;
  bool has_slotted = false;
};

}

void RuleSet::AddRulesFromSheet(const StyleSheetContents* sheet) {
  // 1. pre import layer statement rules

  // 2. import rules

  // 3. child rules
  AddChildRules(nullptr, sheet->GetChildRules());
}

void RuleSet::AddChildRules(StyleRule* parent_rule, const std::vector<A<StyleRuleBase>>& rules) {
  for (auto& i : rules) {
    auto* style_rule = DynamicTo<StyleRule>(i.get());
    if (style_rule) {
      AddStyleRule(style_rule, parent_rule);
    } else {
      assert(false && "TODO");
    }
  }
}

void RuleSet::AddStyleRule(StyleRule* style_rule, StyleRule* parent_rule) {
  int selector_count = style_rule->GetSelectorCount();
  for (int i = 0; i < selector_count; i++) {
    AddRule(style_rule, i);
  }

  if (style_rule->GetChildRuleCount()) {
    AddChildRules(style_rule, style_rule->GetChildRules());
  }
}

void RuleSet::AddRule(StyleRule* style_rule, size_t selector_index) {
  RuleData rule_data(*style_rule, selector_index, m_rule_count);
  m_rule_count++;

  FindBestBucketAndAdd<BucketCoverage::kCompute>(rule_data.MutableSelector(), rule_data);
}

static 
void ExtractBestBucketingValues(const CSSSelector& component, BucketingValues& values) {
  // for (const C)
}

template<RuleSet::BucketCoverage bucket_coverage>
void RuleSet::FindBestBucketAndAdd(CSSSelector& selector, const RuleData& rule_data) {
  BucketingValues values;

  ExtractBestBucketingValues(selector, values);
}




}