#include "html/css/css_default_style_sheets.h"
#include "html/css/style_sheet_contents.h"

namespace html {

CSSDefaultStyleSheets& CSSDefaultStyleSheets::GetInstance() {
  static CSSDefaultStyleSheets s;
  return s;
}

// static 
void CSSDefaultStyleSheets::Init() {
  GetInstance();
}

CSSDefaultStyleSheets::CSSDefaultStyleSheets() {
  InitializeDefaultStyles();
}

void CSSDefaultStyleSheets::InitializeDefaultStyles() {
#if 0
  m_default_html_style.AddRulesFromSheet(
    DefaultStyleSheet()/*, ScreenEval(), {}*/
  ); 
#endif
}

}

