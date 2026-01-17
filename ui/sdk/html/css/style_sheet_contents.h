#ifndef _HTML_CSS_STYLESHEETCONTENTS_H_
#define _HTML_CSS_STYLESHEETCONTENTS_H_

#include "html/base/atomic_string.h"
#include <string>
#include <map>

namespace html {

class StyleSheetContents {
public:
  AtomicString DefaultNamespace() const { return m_default_namespace; }
  const AtomicString NamespaceURIFromPrefix(const AtomicString& prefix) const;

private:
  AtomicString m_default_namespace;

  // PrefixNamespaceURIMap
  std::map<std::u16string, std::u16string> m_namespaces;
};


}

#endif