#ifndef _HTML_CSS_CSSSELECTOR_H_
#define _HTML_CSS_CSSSELECTOR_H_

#include "html/base/atomic_string.h"
#include "html/dom/qualified_name.h"
#include "html/css/parser/css_nesting_type.h"
#include "html/base/memory.h"
#include <string>


namespace html {
class CSSSelectorList;
class StyleRule;

class CSSSelector {
public:
  enum MatchType {
    Unknown,
    InvalidList,   // 作为标记为CSSSelectorList中的结尾项。

    Tag,              // <div>
    UniversalTag,     // *
    Id,               // #id
    Class,            // .class
    PseudoClass,      // :nth-child(2)
    PseudoElement,    // ::first-line
    PageSeudoClass,   // @page :right

    AttributeExact,   // E[foo="bar"],  input[type="submit"]
    AttributeSet,     // E[foo],        只要这个属性存在
    AttributeHyphen,  // E[foo|="bar"], 属性以指定值开头，后跟连字符-，或等于该值, div[class|="btn"]
    AttributeList,    // E[foo~="bar"], 属性值是以空格分隔的列表，其中包含指定值
    AttributeContain, // E[foo*="bar"], 属性值包含指定子字符串
    AttributeBegin,   // E[foo^="bar"], 属性值以指定字符串开头
    AttributeEnd,     // E[foo&="bar"], 属性值以指定字符串结尾
    FirstAttributeSelectorMatch = AttributeExact

  };

  enum class RelationType {
    // .btn.primary, 两个连着的样式，表示同时具备这两个类的元素。
    SubSelector = 0,
    // "Space" 子孙结点
    Descendant,
    // >  子结点
    Child,
    // + 相邻兄弟选择器，紧跟在后面的第一个兄弟元素
    DirectAdjacent,
    // ~ 后面所有的兄弟元素（
    IndirectAdjacent,
  };

  enum class AttributeMatchType : int {
    kCaseSensitive,
    kCaseInsensitive,
    kCaseSensitiveAlways,
  };

  enum PseudoType {
    kUnknown,
    kPseudoActive,
    kPseudoAfter,
    kPseudoBefore,
    kPseudoChecked,
    kPseudoFocus,
    kPseudoHover,
    kPseudoHas,
  };

  CSSSelector() {}
  explicit CSSSelector(MatchType match_type, const QualifiedName &attribute,
                       AttributeMatchType case_sensitivity);
  explicit CSSSelector(MatchType match_type, const QualifiedName &attribute,
                       AttributeMatchType case_sensitivity,
                       const AtomicString &value);

  explicit CSSSelector(const StyleRule *parent_rule, bool is_implicit) {
    assert(false);
  }
  explicit CSSSelector(const AtomicString &pseudo_name, bool is_implicit) {
    assert(false);
  }
  explicit CSSSelector(const QualifiedName& name, bool tag_is_implicit = false) {
    m_tag_q_name_or_attribute = name;
  }

  static const AtomicString& UniversalSelectorAtom() { return g_null_atom; }
public:
  void SetMatchType(MatchType t) { m_match_type = t; }
  MatchType GetMatchType() const { return m_match_type; }

  void SetValue(const AtomicString& value) { m_value = value; }
  void SetRelation(CSSSelector::RelationType relation_type) { m_relation_type = relation_type; }

  void SetLastInSelectorList(bool b) { 
    m_isLastInSelectorList = b;
  }
  bool IsLastInSelectorList() const {
    return m_isLastInSelectorList;
  }
  void SetLastInComplexSelector(bool b) {
    m_isLastInComplexSelector = b;
  }
  bool IsLastInComplexSelector() {
    return m_isLastInComplexSelector;
  }
  void SetScopeContaining(bool b) {
    m_isScopeContaining = b;
  }
  CSSNestingType GetNestingType() const {
    return m_nesting_type;
  }
  const CSSSelectorList* SelectorList() const {
    return m_selector_list;
  }
  PseudoType GetPseudoType() const {
    return m_pseudoType;
  }
  void SetHasArgumentMatchInShadowTree() {
    m_hasArgumentMatchInShadowTree = true;
  }
  const QualifiedName& GetQualifiedName() { return m_tag_q_name_or_attribute; }
  const QualifiedName& GetAttribute() { return m_tag_q_name_or_attribute; }
  const AtomicString& GetValue() { return m_value; }

private:
  MatchType m_match_type = MatchType::Unknown;
  
  QualifiedName m_tag_q_name_or_attribute; 
  AtomicString m_value;

  AttributeMatchType m_case_sensitivity = AttributeMatchType::kCaseInsensitive;
  CSSSelector::RelationType m_relation_type = CSSSelector::RelationType::SubSelector;

  PseudoType m_pseudoType = PseudoType::kUnknown;

  // 用于在CSSSelectorList中遍历对象
  bool m_isLastInSelectorList = false;
  bool m_isLastInComplexSelector = false;
  bool m_isScopeContaining = false;
  bool m_hasArgumentMatchInShadowTree = false;

  CSSNestingType m_nesting_type = CSSNestingType::None;

  CSSSelectorList* m_selector_list = nullptr;
};

}

#endif