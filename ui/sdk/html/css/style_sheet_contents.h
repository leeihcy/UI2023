#ifndef _HTML_CSS_STYLESHEETCONTENTS_H_
#define _HTML_CSS_STYLESHEETCONTENTS_H_

#include "html/base/atomic_string.h"
#include "html/base/memory.h"
#include "html/css/style_rule_import.h"
#include "html/css/style_rule_namespace.h"
#include <cstddef>
#include <string>
#include <map>

namespace html {
class StyleRuleBase;

class StyleSheetContents {
public:
  AtomicString DefaultNamespace() const { return m_default_namespace; }
  const AtomicString NamespaceURIFromPrefix(const AtomicString &prefix) const;
  void ParserAddNamespace(const AtomicString &prefix, const AtomicString &uri);

  void ParserAppendRule(A<StyleRuleBase>&& rule);

  StyleRuleBase* RuleAt(size_t index) const;
  StyleRule* ChildRuleAt(size_t index) const;
  size_t RuleCount() const;
  size_t ChildRuleCount() const;
  size_t NamespaceRuleCount() const { return m_namespace_rules.size(); }
  const std::vector<A<StyleRuleBase>>& GetChildRules() const { return m_child_rules; }
  
private:
  AtomicString m_default_namespace = g_star_atom;

  // PrefixNamespaceURIMap
  std::map<AtomicString, AtomicString> m_namespaces;
  
  std::vector<A<StyleRuleBase>> m_child_rules;
  std::vector<A<StyleRuleNamespace>> m_namespace_rules;
  std::vector<A<StyleRuleImport>> m_import_rules;
  // pre import layer statement rules
};
}

#endif