#ifndef _HTML_CSS_RESOLVER_MATCH_REQUEST_H_
#define _HTML_CSS_RESOLVER_MATCH_REQUEST_H_

#include <cstdint>

namespace html {
class RuleSet;

class RuleSetGroup {
public:
  using RuleSetBitmap = uint32_t;
  // static constexpr unsigned kRulesetsRoom = CHAR_BIT * sizeof(RuleSetBitmap);

  explicit RuleSetGroup(unsigned rule_set_group_index)
      // : m_style_sheet_first_index(rule_set_group_index * kRulesetsRoom) 
      {
      }


  void AddRuleSet(RuleSet* rule_set);

private:
  unsigned m_style_sheet_first_index = 0;
};

}

#endif