#include "html/css/style_sheet_contents.h"

namespace html {

const AtomicString StyleSheetContents::NamespaceURIFromPrefix(
    const AtomicString& prefix) const {
  if (!prefix) {
    return g_null_atom;
  }
  auto it = m_namespaces.find(prefix.text());
  return it != m_namespaces.end() ? AtomicString(it->second.c_str()) : g_null_atom;
}

}