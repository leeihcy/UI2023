#ifndef _HTML_CSS_PARSER_CSSSELECTORPARSER_H_
#define _HTML_CSS_PARSER_CSSSELECTORPARSER_H_

#include "html/css/css_selector.h"
#include "html/base/memory.h"
#include "html/base/atomic_string.h"
#include <string>
#include <vector>

namespace html {
class CSSParserContext;
class StyleSheetContents;

class CSSSelectorParser {
public:
  CSSSelectorParser(const StyleSheetContents* style_sheet) : m_style_sheet(style_sheet) {

  }
  enum ResultFlag {
    kContainsPseudo = 1,

    // contains combinators
    kContainsComplexSelector = 2,

    // :scope, or '&
    kContainsScopeOrParent = 4,
  };
  using ResultFlags = uint8_t;

  static void ConsumeSelector(CSSParserContext& context, StyleSheetContents* style_sheet, std::vector<A<CSSSelector> >& result);
  void ConsumeComplexSelectorList(CSSParserContext& context, std::vector<A<CSSSelector> >& result, ResultFlags& result_flags);

  bool ConsumeSimpleSelector(CSSParserContext& context, ResultFlags& result_flags);
  void ConsumeComplexSelector(CSSParserContext& context, std::vector<A<CSSSelector> >& result, ResultFlags& result_flags);
  void ConsumeCompoundSelector(CSSParserContext& context, std::vector<A<CSSSelector> >& result, ResultFlags& result_flags);
  bool ConsumeName(CSSParserContext& context, AtomicString& name, AtomicString& namespace_prefix);

  bool ConsumeId(CSSParserContext& context);
  bool ConsumeClass(CSSParserContext &context);
  bool ConsumeAttribute(CSSParserContext &context);
  bool ConsumePseudo(CSSParserContext &context, ResultFlags &result_flags);
  bool ConsumeNestinParent(CSSParserContext &context, ResultFlags& result_flags);
  
  const AtomicString DetermineNamespace(const AtomicString& prefix);
  const AtomicString DefaultNamespace() const;

private:
  std::vector<CSSSelector> m_output;
  const StyleSheetContents* m_style_sheet;
};

}

#endif