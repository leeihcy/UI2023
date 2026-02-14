#ifndef _HTML_CSS_CSSDEFAULTSTYLESHEETS_H_
#define _HTML_CSS_CSSDEFAULTSTYLESHEETS_H_

#include "html/base/memory.h"
#include "html/css/rule_set.h"
#include "html/css/style_sheet_contents.h"

namespace html {

class CSSDefaultStyleSheets {
public:
  CSSDefaultStyleSheets();

  static CSSDefaultStyleSheets& GetInstance();
  static void Init();

  void InitializeDefaultStyles();

  RuleSet* DefaultHtmlStyle() { return m_default_html_style.get(); }
  StyleSheetContents* DefaultStyleSheet() { return &m_default_style_sheet; }
  

private:
  A<RuleSet> m_default_html_style = nullptr;

  StyleSheetContents m_default_style_sheet;
};

}

#endif