#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_

#include "html/css/css_selector.h"
#include "html/css/parser/allowed_rules.h"
#include "html/css/parser/css_nesting_type.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
#include "html/css/style_rule.h"
#include "html/css/style_rule_namespace.h"
#include "html/css/style_sheet_contents.h"

#include <optional>
#include <string>
#include <vector>

namespace html {

struct CSSParserContext {
  // 输入内容
  CSSParserTokenStream token_stream;

  // css内容解析结果
  // StyleSheetContents style_sheet;

  // style=""内容解析结果
  std::vector<CSSPropertyValue> parsed_properties;

  StyleSheetContents *style_sheet = nullptr;

  // CSSNestingType nesting_type = CSSNestingType::None;
  // const StyleRule* parent_rule_for_nesting = nullptr;
  uint8_t result_flags = 0;
};

class CSSParser {
public:
  std::unique_ptr<CSSPropertyValueSet>
  ParseInlineStyleDeclaration(const char *bytes, size_t size);

  bool ParseStyleSheet(StyleSheetContents *style_sheet, const char *bytes,
                       size_t size);

protected:
  void ConsumeBlockContents(CSSParserContext &context,
                            StyleRule::RuleType rule_type, CSSNestingType,
                            StyleRule *parent_rule_for_nesting,
                            size_t nested_declarations_start_index,
                            std::vector<A<StyleRuleBase>> *child_rules);
  bool ConsumeDeclaration(CSSParserContext &context,
                          StyleRule::RuleType rule_type);
  void ConsumeDeclarationValue(CSSParserContext &context,
                               CSSPropertyID property_id);

  bool ConsumeRuleList(CSSParserContext &context, AllowedRules allowed_rules,
                       CSSNestingType nesting_type,
                       StyleRule *parent_rule_for_nesting);
  void onConsumeRuleList(CSSParserContext &context, A<StyleRuleBase> &&rule,
                         unsigned int offset);

  A<StyleRuleBase> ConsumeAtRule(CSSParserContext &stream,
                                 AllowedRules allowed_rules);
  A<StyleRuleBase> ConsumeAtRuleContents(CSSAtRuleID id,
                                         CSSParserTokenStream &stream,
                                         AllowedRules allowed_rules);
  A<StyleRuleBase> ConsumeNestedRule(std::optional<CSSAtRuleID> id,
                                     StyleRule::RuleType parent_rule_type,
                                     CSSParserContext &context,
                                     CSSNestingType nesting_type,
                                     StyleRule *parent_rule_for_nesting,
                                     bool &invalid_rule_error);

  void EmitDeclarationsRuleIfNeeded(CSSParserContext &context,
                                    StyleRule::RuleType rule_type,
                                    CSSNestingType nesting_type,
                                    StyleRule *parent_rule_for_nesting,
                                    size_t start_index,
                                    std::vector<A<StyleRuleBase>> &child_rules);

  A<StyleRuleBase> CreateDeclarationsRule(CSSParserContext &context,
                                          CSSNestingType nesting_type,
                                          const CSSSelector *selector_list,
                                          size_t start_index);
  void ConsumeErroneousAtRule(CSSParserTokenStream &stream, CSSAtRuleID id);

  bool ConsumeVariableValue(CSSParserContext &context,
                            const AtomicString &variable_name,
                            bool allow_important_annotation,
                            bool is_animation_tainted);

  A<StyleRuleBase> ConsumeQualifiedRule(CSSParserContext &context,
                                        AllowedRules allowed_rules,
                                        CSSNestingType nesting_type,
                                        StyleRule *parent_rule_for_nesting);
  A<StyleRuleBase> ConsumeStyleRule(CSSParserContext &context,
                                    CSSNestingType nesting_type,
                                    StyleRule *parent_rule_for_nesting,
                                    bool nested, bool &invalid_rule_error);
  A<StyleRuleNamespace> ConsumeNamespaceRule(CSSParserTokenStream &stream);
  A<StyleRuleBase>
  ConsumeStyleRuleContents(const std::vector<CSSSelector> &selector_vector,
                           CSSParserContext &context, bool has_visited_pseudo);
  bool ConsumeEndOfPreludeForAtRuleWithoutBlock(CSSParserTokenStream &stream,
                                                CSSAtRuleID id);
  bool ConsumeEndOfPreludeForAtRuleWithBlock(CSSParserTokenStream &stream,
                                             CSSAtRuleID id);

public:
  static constexpr AllowedRules kConditionalRules = {
      CSSAtRuleID::kCSSAtRuleMedia,
      CSSAtRuleID::kCSSAtRuleSupports,
      CSSAtRuleID::kCSSAtRuleContainer,
      CSSAtRuleID::kCSSAtRuleNavigation,
  };
  static constexpr AllowedRules kNestedGroupRules =
      kConditionalRules | AllowedRules{
                              CSSAtRuleID::kCSSAtRuleLayer,
                              CSSAtRuleID::kCSSAtRuleScope,
                              CSSAtRuleID::kCSSAtRuleStartingStyle,
                              CSSAtRuleID::kCSSAtRuleViewTransition,
                              CSSAtRuleID::kCSSAtRuleApplyMixin,
                          };
  // Valid rules within @page.
  static constexpr AllowedRules kPageMarginRules = {
      CSSAtRuleID::kCSSAtRuleTopLeftCorner,
      CSSAtRuleID::kCSSAtRuleTopLeft,
      CSSAtRuleID::kCSSAtRuleTopCenter,
      CSSAtRuleID::kCSSAtRuleTopRight,
      CSSAtRuleID::kCSSAtRuleTopRightCorner,
      CSSAtRuleID::kCSSAtRuleBottomLeftCorner,
      CSSAtRuleID::kCSSAtRuleBottomLeft,
      CSSAtRuleID::kCSSAtRuleBottomCenter,
      CSSAtRuleID::kCSSAtRuleBottomRight,
      CSSAtRuleID::kCSSAtRuleBottomRightCorner,
      CSSAtRuleID::kCSSAtRuleLeftTop,
      CSSAtRuleID::kCSSAtRuleLeftMiddle,
      CSSAtRuleID::kCSSAtRuleLeftBottom,
      CSSAtRuleID::kCSSAtRuleRightTop,
      CSSAtRuleID::kCSSAtRuleRightMiddle,
      CSSAtRuleID::kCSSAtRuleRightBottom,
  };

private:
  std::vector<CSSSelector> m_arena;
  bool m_in_nested_style_rule = false;
  bool in_mixin_ = false;
};
} // namespace html

#endif