#ifndef _HTML_CSS_RESOLVER_CONTEXT_H_
#define _HTML_CSS_RESOLVER_CONTEXT_H_

namespace html {
class Element;

class  ElementResolveContext {
public:
  explicit ElementResolveContext(Element&);

  Element& GetElement() const { return m_element; }

private:
  Element& m_element;
};

}

#endif