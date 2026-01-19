#include "html/css/style_sheet_contents.h"

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

}