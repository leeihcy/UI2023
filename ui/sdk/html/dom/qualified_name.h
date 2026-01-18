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
  QualifiedName(AtomicString localname) : m_localname(localname) {}
  QualifiedName(AtomicString prefix, AtomicString localname,
                AtomicString namespace_uri)
      : m_prefix(prefix), m_localname(localname),
        m_namespace_uri(namespace_uri) {}

private:
  AtomicString m_namespace_uri;
  AtomicString m_prefix;
  AtomicString m_localname;
};

}

#endif