#ifndef _HTML_CSS_CSSSELECTORLIST_H_
#define _HTML_CSS_CSSSELECTORLIST_H_

#include "html/css/css_selector.h"
#include <vector>


namespace html {

class CSSSelectorList {
public:
  CSSSelectorList() {
  }
  static bool IsValid(const CSSSelector &first) {
    return first.GetMatchType() != CSSSelector::InvalidList;
  }
  bool IsValid() const {
    if (m_selectors.empty()) {
      return false;
    }
    return IsValid(m_selectors[0]);
  }
  const CSSSelector *First() const {
    return IsValid() ? &m_selectors[0] : nullptr;
  }

private:
  std::vector<CSSSelector> m_selectors;
};

}

#endif