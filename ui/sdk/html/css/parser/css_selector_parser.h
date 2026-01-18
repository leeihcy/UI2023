#ifndef _HTML_CSS_PARSER_CSSSELECTORPARSER_H_
#define _HTML_CSS_PARSER_CSSSELECTORPARSER_H_

#include "html/css/css_selector.h"
#include "html/css/parser/css_nesting_type.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/base/memory.h"
#include "html/base/atomic_string.h"
#include <string>
#include <vector>
#include <span>

namespace html {
class StyleSheetContents;

struct CSSSelectorParserContext {
  CSSParserTokenStream token_stream;
  CSSNestingType nesting_type = CSSNestingType::None;
  // const StyleRule* parent_rule_for_nesting_;
  uint8_t result_flags = 0;
};

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

  static void ConsumeSelector(CSSSelectorParserContext& context, StyleSheetContents* style_sheet, std::vector<CSSSelector>& result);
  std::span<CSSSelector> ConsumeComplexSelectorList(CSSSelectorParserContext& context);

  bool ConsumeSimpleSelector(CSSSelectorParserContext& context);
  std::span<CSSSelector> ConsumeComplexSelector(CSSSelectorParserContext& context);
  std::span<CSSSelector> ConsumeCompoundSelector(CSSSelectorParserContext& context);
  bool ConsumeName(CSSSelectorParserContext& context, AtomicString& name, AtomicString& namespace_prefix);

  bool ConsumeId(CSSSelectorParserContext& context);
  bool ConsumeClass(CSSSelectorParserContext &context);
  bool ConsumeAttribute(CSSSelectorParserContext &context);
  bool ConsumePseudo(CSSSelectorParserContext &context, ResultFlags &result_flags);
  bool ConsumeNestinParent(CSSSelectorParserContext &context,
                           ResultFlags &result_flags);
  bool ConsumePartialComplexSelector(CSSSelectorParserContext &context,
                                     CSSSelector::RelationType &combinator,
                                     unsigned previous_compound_flags);
  bool PeekIsCombinator(CSSParserTokenStream& stream);
  std::span<CSSSelector> ConsumeNestedRelativeSelector(CSSSelectorParserContext &context);
  void SplitCompoundAtImplicitCombinator(std::span<CSSSelector> selectors);

  CSSSelector::RelationType ConsumeCombinator(CSSSelectorParserContext& context);
  CSSSelector::MatchType ConsumeAttributeMatch(CSSSelectorParserContext &context);
  CSSSelector::AttributeMatchType ConsumeAttributeFlags(CSSSelectorParserContext &context);

  const AtomicString DetermineNamespace(const AtomicString& prefix);
  const AtomicString DefaultNamespace() const;

  class ResetVectorAfterScope {
   public:
    explicit ResetVectorAfterScope(std::vector<CSSSelector>& vector)
        : vector_(vector), initial_size_(vector.size()) {}
    ~ResetVectorAfterScope() {
      if (!committed_) {
        vector_.resize(initial_size_);
      }
    }

    // 返回本轮新增的那几个对象视图。
    std::span<CSSSelector> AddedElements() const {
      assert(vector_.size() >= initial_size_);
      return std::span<CSSSelector>(vector_).subspan(initial_size_);
    }

    std::span<CSSSelector> CommitAddedElements() {
      committed_ = true;
      return AddedElements();
    }

   private:
    std::vector<CSSSelector>& vector_;
    const size_t initial_size_;
    bool committed_ = false;
  };

private:
  bool m_failed_parsing = false;
  std::vector<CSSSelector> m_output;
  const StyleSheetContents* m_style_sheet;
  const StyleRule* parent_rule_for_nesting_ = nullptr;
  CSSSelector::PseudoType restricting_pseudo_element_;
  bool found_host_in_compound_ = false;
};

}

#endif