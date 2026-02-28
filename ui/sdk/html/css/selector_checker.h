#ifndef _HTML_CSS_SELECTOR_CHECKER_H_
#define _HTML_CSS_SELECTOR_CHECKER_H_

#include "html/css/css_selector.h"
#include "html/dom/element.h"
namespace html {

class SelectorChecker {

};

class EasySelectorChecker {
public:
  static bool IsEasy(const CSSSelector* selector);
  static bool Match(const CSSSelector* selector, const Element* element);

private:
  static bool MatchOne(const CSSSelector* selector, const Element* element);
};


}

#endif