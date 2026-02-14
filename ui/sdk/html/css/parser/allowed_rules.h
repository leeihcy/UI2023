#ifndef _HTML_CSS_PARSER_ALLOWDRULES_H_
#define _HTML_CSS_PARSER_ALLOWDRULES_H_

#include "html/css/parser/css_at_rule_id.h"
#include <cstdint>
#include <initializer_list>

namespace html {

enum class QualifiedRuleType {
  // 普通样式，如 .foo:hover {}
  Style,
  // 关键帧{}中的样式， 如 50% {}
  Keyframe,

  Count
};

// 在不同的上下文中，可以解析的样式不一样。
// 例如：
// 1. 只有在根目录下，才支持@import
// 2. 在@keyframes下，才支持from/to, 0% 100%
//
class AllowedRules {
public:
  template<typename T>
  static constexpr uint64_t EnumBits(std::initializer_list<T> list) {
    unsigned int bits = 0;
    for (T item : list) {
      bits |= 1 << (unsigned int)item;
    }
    return bits;
  }

  constexpr AllowedRules() {}
  
  constexpr AllowedRules(std::initializer_list<QualifiedRuleType> list) :
    m_qualified_bits(EnumBits(list)) {
  }
  constexpr AllowedRules(std::initializer_list<CSSAtRuleID> list) :
    m_atrule_bits(EnumBits(list)) {
  }
  constexpr AllowedRules operator|(const AllowedRules& o) const {
    return AllowedRules(m_atrule_bits|o.m_atrule_bits, m_qualified_bits|o.m_qualified_bits); 
  }

  bool Has(CSSAtRuleID id) const {
    return (m_atrule_bits >> (int)id) & 0x1;
  }
  bool Has(QualifiedRuleType id) const {
    return (m_qualified_bits >> (int)id) & 0x1;
  }

  void Remove(CSSAtRuleID id) {
    m_atrule_bits &= ~(int)id;
  }
  void Remove(QualifiedRuleType id) {
    m_qualified_bits &= ~(int)id;
  }
private:
  explicit constexpr AllowedRules(uint64_t atrule_bits, uint64_t qualified_bits) :
    m_atrule_bits(atrule_bits), m_qualified_bits(qualified_bits) {}

private:
  uint64_t m_atrule_bits : 60 = 0;
  uint64_t m_qualified_bits : 4 = 0;
};


static constexpr AllowedRules kRegularRules = 
  AllowedRules{QualifiedRuleType::Style} | 
  AllowedRules{
    CSSAtRuleID::FontFace,
    CSSAtRuleID::Keyframes,
    CSSAtRuleID::kCSSAtRuleScope
    // ...
  };

// 最外层rules。例如不能在其它规则下使用@import
static constexpr AllowedRules kTopLevelRules = 
  kRegularRules | AllowedRules{
    CSSAtRuleID::Charset,
    CSSAtRuleID::Import,
    CSSAtRuleID::Namespace
  };

}

#endif