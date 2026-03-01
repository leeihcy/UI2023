#ifndef _HTML_CSS_RESOLVER_STYLERESOLVER_STATE_H_
#define _HTML_CSS_RESOLVER_STYLERESOLVER_STATE_H_

#include "html/dom/document.h"
#include "html/dom/element.h"
#include "html/css/resolver/style_resolver.h"
#include "html/css/resolver/element_resolve_context.h"

namespace html {

class StyleResolverState {
public:
  StyleResolverState(Document&,
                     Element&,
                     const StyleRecalcContext* = nullptr,
                     const StyleRequest& = StyleRequest());
  Element& GetElement() const { return m_element_context.GetElement(); }

  const ComputedStyle* Style() const { return style_.get(); }
  ComputedStyle* Style() { return style_.get(); }

  std::shared_ptr<ComputedStyle> TakeStyle() {
    return style_;
  }

private:
  ElementResolveContext  m_element_context;
  Document* m_document;
  const StyleRecalcContext* m_style_recalc_context;


  // style_ is the primary output for each element's style resolve.
  std::shared_ptr<ComputedStyle> style_;
};

}

#endif