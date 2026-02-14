#ifndef _HTML_CSS_RESOLVER_STYLERESOLVER_STATE_H_
#define _HTML_CSS_RESOLVER_STYLERESOLVER_STATE_H_

#include "html/dom/element.h"
#include "html/css/resolver/style_resolver.h"
#include "html/css/resolver/element_resolve_context.h"

namespace html {

class StyleResolverState {
public:
  Element& GetElement() const { return m_element_context.GetElement(); }

private:
  ElementResolveContext  m_element_context;
};

}

#endif