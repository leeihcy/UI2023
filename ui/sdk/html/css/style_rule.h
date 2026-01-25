#ifndef _HTML_CSS_STYLERULE_H_
#define _HTML_CSS_STYLERULE_H_

#include "html/base/casting.h"
#include "html/base/memory.h"
#include "html/css/css_property_value.h"
#include "html/css/css_selector.h"
#include "html/css/css_property_value_set.h"
#include <assert.h>
#include <vector>

namespace html {

class StyleRuleBase {
public:
  enum RuleType {
    kCharset,
    kStyle,
    kImport,
    kFontFace,
    kKeyframes,
    kKeyframe,
    kNamespace,
    kLayerStatement,
    kScope,
    kPage,
    kFunction,



    
    kFontPaletteValues,
    kProperty,
    kRoute,
    kCounterStyle,
    kViewTransition,
  };

public:
  RuleType GetType() const { return m_type; }
  bool IsCharsetRule() const { return m_type == kCharset; }
  bool IsLayerStatementRule() const { return m_type ==  kLayerStatement; }
  bool IsImportRule() const { return m_type == kImport; }
  bool IsNamespaceRule() const { return m_type == kNamespace; }
  
protected:
  explicit StyleRuleBase(RuleType type) : m_type(type) {}

private:
  RuleType m_type;
};

class StyleRule : public StyleRuleBase {
public:
  const CSSSelector* FirstSelector() const { 
    return m_selectors.data();
  }
  static A<StyleRule> Create(const std::vector<CSSSelector>& selectors) {
    return A<StyleRule>::make_new(selectors);
  }

  StyleRule(const std::vector<CSSSelector>& selectors) : 
    m_selectors(selectors), StyleRuleBase(kStyle) {

  }
  
  void AddChildRule(A<StyleRuleBase>&& child) {
    m_child_rules.push_back(std::move(child));
  }
  void SetProperties(std::vector<CSSPropertyValue>&& properties) {
    m_properties.reset(A<CSSPropertyValueSet>::make_new(std::move(properties)));
  }
private:
  A<CSSPropertyValueSet> m_properties = nullptr;
  std::vector<A<StyleRuleBase>> m_child_rules;
  std::vector<CSSSelector> m_selectors;
};

template<>
struct DowncastTraits<StyleRule> {
  static bool AllowFrom(const StyleRuleBase& from) {
    return from.GetType() == StyleRuleBase::kStyle;
  }
};

}

#endif