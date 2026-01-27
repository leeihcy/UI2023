#include "html/css/style_sheet_contents.h"
#include "html/base/casting.h"
#include "html/css/style_rule_namespace.h"
#include "html/css/style_rule_import.h"

namespace html {

const AtomicString StyleSheetContents::NamespaceURIFromPrefix(
    const AtomicString& prefix) const {
  if (!prefix) {
    return g_null_atom;
  }
  auto it = m_namespaces.find(prefix.text());
  return it != m_namespaces.end() ? it->second : g_null_atom;
}

void StyleSheetContents::ParserAddNamespace(const AtomicString& prefix,
                                            const AtomicString& uri) {
  assert(!uri.IsNull());
  if (prefix.IsNull()) {
    m_default_namespace = uri;
    return;
  }
  m_namespaces[prefix] = uri;
}

void StyleSheetContents::ParserAppendRule(A<StyleRuleBase>&& rule) {
#if 0
  if (auto* layer_statement_rule = DynamicTo<StyleRuleLayerStatement>(rule)) {
    if (import_rules_.empty() && namespace_rules_.empty() &&
        child_rules_.empty()) {
      pre_import_layer_statement_rules_.push_back(layer_statement_rule);
      return;
    }
    // Falls through, insert it into child_rules_ as a regular rule
  }
#endif

  if (auto* p = DynamicTo<StyleRuleImport>(rule.get())) {
    A<StyleRuleImport> import_rule = nullptr;
    import_rule.reset(std::move(rule));
    
    // Parser enforces that @import rules come before anything else other than
    // empty layer statements
    assert(m_child_rules.empty());
#if 0    
    if (import_rule->MediaQueries()) {
      SetHasMediaQueries();
    }
#endif
    m_import_rules.push_back(std::move(import_rule));
    m_import_rules.back()->SetParentStyleSheet(this);
    m_import_rules.back()->RequestStyleSheet();
    return;
  }

  if (auto* p = DynamicTo<StyleRuleNamespace>(rule.get())) {
    A<StyleRuleNamespace> namespace_rule = nullptr;
    namespace_rule.reset(std::move(rule));
    // Parser enforces that @namespace rules come before all rules other than
    // import/charset rules and empty layer statements
    assert(m_child_rules.empty());
    ParserAddNamespace(namespace_rule->Prefix(), namespace_rule->Uri());
    m_namespace_rules.push_back(std::move(namespace_rule));
    return;
  }

  m_child_rules.push_back(std::move(rule));
}

StyleRule* StyleSheetContents::RuleAt(size_t index) const {
  if (index >= m_child_rules.size()) {
    return nullptr;
  }
  return DynamicTo<StyleRule>(m_child_rules[index].get());
}

}