#include "html/css/resolver/style_resolver_state.h"
#include "html/style/computed_style.h"

namespace html {

StyleResolverState::StyleResolverState(
    Document& document,
    Element& element,
    const StyleRecalcContext* style_recalc_context,
    const StyleRequest& style_request) :
     m_element_context(element),
      m_style_recalc_context(style_recalc_context),
      m_document(&document) {
  style_ = std::make_shared<ComputedStyle>();
}

}