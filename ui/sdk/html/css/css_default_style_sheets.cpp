#include "html/css/css_default_style_sheets.h"
#include "html/css/parser/css_parser.h"
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
  static const char* default_style_css = R"(
  html, body {}
  #btn { background-color: red; }
)";
  html::CSSParser parser;
  parser.ParseStyleSheet(&m_default_style_sheet, default_style_css, strlen(default_style_css));

  if (!m_default_html_style) {
    m_default_html_style.reset(std::move(A<RuleSet>::make_new()));
    m_default_html_style->AddRulesFromSheet(
        DefaultStyleSheet() /*, ScreenEval(), {}*/
    );
  }
}
}

