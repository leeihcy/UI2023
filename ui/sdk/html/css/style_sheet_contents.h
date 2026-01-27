#ifndef _HTML_CSS_STYLESHEETCONTENTS_H_
#define _HTML_CSS_STYLESHEETCONTENTS_H_

#include "html/base/atomic_string.h"
#include "html/base/memory.h"
#include "html/css/style_rule_import.h"
#include <string>
#include <map>

namespace html {
class StyleRuleBase;
class StyleRuleNamespace;

class StyleSheetContents {
public:
  AtomicString DefaultNamespace() const { return m_default_namespace; }
  const AtomicString NamespaceURIFromPrefix(const AtomicString &prefix) const;
  void ParserAddNamespace(const AtomicString &prefix, const AtomicString &uri);

  void ParserAppendRule(A<StyleRuleBase>&& rule);

  StyleRule* RuleAt(size_t index) const;
  size_t RuleCount() const { return m_child_rules.size(); }

private:
  AtomicString m_default_namespace = g_star_atom;

  // PrefixNamespaceURIMap
  std::map<AtomicString, AtomicString> m_namespaces;
  
  std::vector<A<StyleRuleBase>> m_child_rules;
  std::vector<A<StyleRuleNamespace>> m_namespace_rules;
  std::vector<A<StyleRuleImport>> m_import_rules;
};
}

#endif