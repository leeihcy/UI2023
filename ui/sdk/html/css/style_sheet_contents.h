#ifndef _HTML_CSS_STYLESHEETCONTENTS_H_
#define _HTML_CSS_STYLESHEETCONTENTS_H_

#include "html/base/atomic_string.h"
#include <string>
#include <map>

namespace html {

class StyleSheetContents {
public:
  AtomicString DefaultNamespace() const { return m_default_namespace; }
  const AtomicString NamespaceURIFromPrefix(const AtomicString &prefix) const;
  void ParserAddNamespace(const AtomicString &prefix, const AtomicString &uri);

private:
  AtomicString m_default_namespace = g_star_atom;

  // PrefixNamespaceURIMap
  std::map<AtomicString, AtomicString> m_namespaces;
};
}

#endif