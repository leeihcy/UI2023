#ifndef _HTML_CSS_RESOLVER_MATCH_REQUEST_H_
#define _HTML_CSS_RESOLVER_MATCH_REQUEST_H_

#include <cstdint>
#include <vector>
#include <bit>
#include "html/base/memory.h"

namespace html {
class RuleSet;
class ContainerNode;
class Element;

class RuleSetGroup {
public:
  using RuleSetBitmap = uint32_t;
  // static constexpr unsigned kRulesetsRoom = CHAR_BIT * sizeof(RuleSetBitmap);

  explicit RuleSetGroup(unsigned rule_set_group_index)
      // : m_style_sheet_first_index(rule_set_group_index * kRulesetsRoom) 
      {
      }


  void AddRuleSet(const RuleSet* rule_set);
  bool IsEmpty() { return m_rule_sets.empty(); }

private:
public:
  unsigned m_style_sheet_first_index = 0;
  std::vector<const RuleSet*>  m_rule_sets;
  RuleSetBitmap single_scope_ = 0;
  RuleSetBitmap not_single_scope_ = 0;
};

class MatchRequest {
public:
  MatchRequest(const RuleSetGroup &rule_set_group, const ContainerNode *scope)
      : rule_set_group_(rule_set_group), scope_(scope),
        enabled_(rule_set_group.not_single_scope_) {}

  // Used for returning from RuleSetIterator; not actually stored.
  struct RuleSetWithIndex {
    // STACK_ALLOCATED();

   public:
    const RuleSet* rule_set;
    unsigned style_sheet_index;
  };

   // An iterator over all the rule sets in a given bitmap, intended for use in
  // range-based for loops (use AllRuleSets()). The index is automatically
  // generated based on style_sheet_first_index_. RuleSets that are not part of
  // the bitmap are skipped over at no cost.
  class RuleSetIterator {
    // STACK_ALLOCATED();

   public:
    RuleSetIterator(const RuleSetGroup* rule_set_group,
                    RuleSetGroup::RuleSetBitmap bitmap)
        : rule_set_group_(*rule_set_group), bitmap_(bitmap) {}

    RuleSetWithIndex operator*() const {
      // DCHECK_NE(0u, bitmap_);
      unsigned index = std::countr_zero(bitmap_);
      return {rule_set_group_.m_rule_sets[index],
              index + rule_set_group_.m_style_sheet_first_index};
    }

    RuleSetIterator& operator++() {
      bitmap_ &= bitmap_ - 1;
      return *this;
    }

    bool operator==(const RuleSetIterator& other) const {
      // DCHECK_EQ(&rule_set_group_, &other.rule_set_group_);
      return bitmap_ == other.bitmap_;
    }

   private:
    const RuleSetGroup& rule_set_group_;
    RuleSetGroup::RuleSetBitmap bitmap_;
  };

  // A proxy object to allow AllRuleSets() to be iterable in a range-based
  // for loop (ie., provide begin() and end() member functions).
  class RuleSetIteratorProxy {
    // STACK_ALLOCATED();

   public:
    RuleSetIteratorProxy(const RuleSetGroup* rule_set_group,
                         RuleSetGroup::RuleSetBitmap bitmap)
        : rule_set_group_(*rule_set_group), bitmap_(bitmap) {}

    RuleSetIterator begin() const { return {&rule_set_group_, bitmap_}; }
    RuleSetIterator end() const { return {&rule_set_group_, 0}; }

   private:
    const RuleSetGroup& rule_set_group_;
    RuleSetGroup::RuleSetBitmap bitmap_;
  };


  RuleSetIteratorProxy AllRuleSets() const {
    return RuleSetIteratorProxy(&rule_set_group_, enabled_);
  }

private:
  const RuleSetGroup& rule_set_group_;
  const ContainerNode* const scope_;

  // Always set for every RuleSet added, except those currently disabled by the
  // single-scope optimization.
  RuleSetGroup::RuleSetBitmap enabled_;
};

}

#endif