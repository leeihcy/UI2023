#ifndef _HTML_DOM_QUALIFIED_NAME_H_
#define _HTML_DOM_QUALIFIED_NAME_H_

#include "html/base/atomic_string.h"
#include <string>

namespace html {

// QualifiedName 是用于在命名空间（namespace）下唯一标识元素或属性的数据结构
//
// 命名空间，例如http://www.w3.org/1999/xhtml (HTML)
// 前缀，在文档中引用命名空间的简短别名，方便书写。例如svg, xlink
// 本地名称，例如href, style, circle

class QualifiedName {
public:
  QualifiedName() {}
  QualifiedName(AtomicString localname) : m_localname(std::move(localname)) {}
  QualifiedName(AtomicString prefix, AtomicString localname,
                AtomicString namespace_uri)
      : m_prefix(std::move(prefix)), m_localname(std::move(localname)),
        m_namespace_uri(std::move(namespace_uri)) {}

  const AtomicString& NamespaceUri() const { return m_namespace_uri; }
  const AtomicString& Prefix() const { return m_prefix; }
  const AtomicString& LocalName() const { return m_localname; }

  bool operator==(const QualifiedName &other) {
    return m_namespace_uri == other.m_namespace_uri &&
           m_prefix == other.m_prefix && m_localname == other.m_localname;
  }
  bool operator!=(const QualifiedName& other) {
    return !(*this == other);
  }
private:
  AtomicString m_namespace_uri;
  AtomicString m_prefix;
  AtomicString m_localname;
};

extern const QualifiedName g_any_name;
extern const QualifiedName g_null_name;

inline bool operator==(const AtomicString& a, const QualifiedName& q) {
  return a == q.LocalName();
}
inline bool operator!=(const AtomicString& a, const QualifiedName& q) {
  return a != q.LocalName();
}
inline bool operator==(const QualifiedName& q, const AtomicString& a) {
  return a == q.LocalName();
}
inline bool operator!=(const QualifiedName& q, const AtomicString& a) {
  return a != q.LocalName();
}

}


#endif