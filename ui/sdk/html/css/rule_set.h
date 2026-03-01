#ifndef _HTML_CSS_RULE_SET_H_
#define _HTML_CSS_RULE_SET_H_

#include "html/base/atomic_string.h"
#include "html/css/css_selector.h"
#include "html/css/selector_checker.h"
#include "html/css/style_rule.h"
#include "html/css/style_rule_import.h"
#include "html/css/style_scope.h"
#include "html/css/robin_hood_map-inl.h"
#include <span>
#include <vector>

namespace html {



// 将 StyleRule 中的数据，按 selector 维度拆分开来，每项做为一个RuleData。
class RuleData {
public:
  RuleData(StyleRule& style_rule, unsigned selector_index, unsigned position) :
    m_style_rule(style_rule), m_selector_index(selector_index), m_position(position)
  {}
  
  StyleRule* Rule() const { return &m_style_rule; }

  CSSSelector& MutableSelector() {
    return m_style_rule.GetMutableSelectorAt(m_selector_index);
  }
  const CSSSelector& Selector() const {
    return *m_style_rule.GetSelectorAt(m_selector_index);
  }
  bool SelectorIsEasy() const {
    return EasySelectorChecker::IsEasy(&Selector());
  }
private:
  StyleRule& m_style_rule;
  unsigned m_selector_index;
  unsigned m_position;
};


class RuleMap {
  // DISALLOW_NEW();
  // A collection of rules that are in the same bucket. Before compaction,
  // they are scattered around in the bucket vector; after compaction,
  // each bucket will be contiguous.
  struct Extent {
    Extent() : bucket_number(0) {}
    union {
      // [0..num_buckets). Valid before compaction.
      unsigned bucket_number;

      // Into the backing vector. Valid after compaction.
      unsigned start_index;
    };

    // How many rules are in this bucket. Will naturally not change
    // by compaction.
    size_t length = 0;
  };
public:
   std::span<const RuleData> Find(const AtomicString& key) const {
     const RobinHoodMap<AtomicString, Extent>::Bucket* bucket =
        buckets.Find(key);
    if (bucket == nullptr) {
      return {};
    } else {
      const RuleData* start = &(*backing.begin()) + bucket->value.start_index;
      return std::span<const RuleData>(start, bucket->value.length);
    }
   }


  bool IsEmpty() const { return backing.empty(); }
  bool IsCompacted() const { return compacted; }

  void Compact() {
    // TODO: 
  }
  void Uncompact() {
    // TODO:
  }

  bool Add(const AtomicString& key, const RuleData& rule_data) {
    RuleMap::Extent *rules = nullptr;
    if (buckets.IsNull()) {
      // First insert.
      buckets = RobinHoodMap<AtomicString, Extent>(8);
    } else {
      // See if we can find an existing entry for this key.
      RobinHoodMap<AtomicString, Extent>::Bucket *bucket = buckets.Find(key);
      if (bucket != nullptr) {
        rules = &bucket->value;
      }
    }
    if (rules == nullptr) {
      RobinHoodMap<AtomicString, Extent>::Bucket *bucket = buckets.Insert(key);
      if (bucket == nullptr) {
        return false;
      }
      rules = &bucket->value;
      rules->bucket_number = num_buckets++;
    }

    RuleData rule_data_copy = rule_data;
    // rule_data_copy.ComputeEntirelyCoveredByBucketing();
    bucket_number_.push_back(rules->bucket_number);
    ++rules->length;
    backing.push_back(std::move(rule_data_copy));
    return true;
  }
 private:

  RobinHoodMap<AtomicString, Extent> buckets;
  std::vector<RuleData> backing;
  std::vector<unsigned> bucket_number_;

  unsigned int num_buckets = 0;
  bool compacted = false;
};

class RuleSet {
public:
  void AddRulesFromSheet(const StyleSheetContents* sheet, const StyleScope* style_scope=nullptr);
  void AddChildRules(StyleRule* parent_rule,
     const std::vector<A<StyleRuleBase>>& rules,  const StyleScope* style_scope);
  void AddStyleRule(StyleRule* style_rule, StyleRule* parent_rule, 
    const StyleScope* style_scope);
  void AddRule(StyleRule* style_rule, unsigned selector_index, const StyleScope* style_scope);


  enum class BucketCoverage {
    kIgnore,
    kCompute,
  };
  template<BucketCoverage bucket_coverage>
  void FindBestBucketAndAdd(CSSSelector&, const RuleData&,const StyleScope* style_scope);
  

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

  
  std::span<const RuleData> IdRules(const AtomicString& key) const {
    return id_rules_.Find(key);
  }

  void AddToBucket(std::vector<RuleData>& rules,
                          const RuleData& rule_data);
  void AddToBucket(const AtomicString& key,
                          RuleMap& map,
                          const RuleData& rule_data);
private:
  unsigned m_rule_count = 0;

  RuleMap id_rules_;
  RuleMap class_rules_;
  RuleMap attr_rules_;

  std::vector<RuleData> universal_rules_;

  bool need_compaction_ = false;
};

}

#endif