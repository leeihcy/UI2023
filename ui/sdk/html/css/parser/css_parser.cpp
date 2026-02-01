#include "html/css/parser/css_parser.h"

#include "html/css/css_property_value_set.h"
#include "html/css/parser/allowed_rules.h"
#include "html/css/parser/css_at_rule_id.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_property_parser.h"
#include "html/css/parser/css_selector_parser.h"
#include "html/css/css_variable_data.h"
#include "html/css/parser/css_variable_parser.h"
#include "html/css/property/property_id.h"
#include "html/css/property/value_id.h"
#include "html/css/style_rule_namespace.h"
#include "html/css/style_rule.h"
#include "html/util/util.h"
#include "html/base/auto_reset.h"
#include <assert.h>
#include <string>
#include <vector>

#define kNotFound (uint32_t)-1


namespace html {

// 解析style=""内容
std::unique_ptr<CSSPropertyValueSet>
CSSParser::ParseInlineStyleDeclaration(const char *bytes, size_t size) {
  CSSParserContext context;
  context.token_stream.SetInput(bytes, size);

  ConsumeBlockContents(context, StyleRule::kStyle, 
     CSSNestingType::None, nullptr, kNotFound, nullptr);
  return std::make_unique<CSSPropertyValueSet>(
      std::move(context.parsed_properties));
}

// 解析 { } 里的内容。
void CSSParser::ConsumeBlockContents(
    CSSParserContext &context, StyleRule::RuleType rule_type,
    CSSNestingType nesting_type,
    StyleRule* parent_rule_for_nesting,
    size_t nested_declarations_start_index,
    std::vector<A<StyleRuleBase>> *child_rules) {
  auto &stream = context.token_stream;

  while (true) {
    if (context.token_stream.AtEnd()) {
      break;
    }

    switch (context.token_stream.Peek().Type()) {
    case CSSParserTokenType::Whitespace:
    case CSSParserTokenType::Semicolon:
      context.token_stream.Consume();
      break;

    case CSSParserTokenType::AtKeyword: {
      CSSParserToken name_token = stream.ConsumeIncludingWhitespace();
        const std::u16string& name = name_token.Name();
        const CSSAtRuleID id = CssAtRuleID(name);
        bool invalid_rule_error_ignored = false;
        A<StyleRuleBase> child = ConsumeNestedRule(
            id, rule_type, context, nesting_type, parent_rule_for_nesting,
            invalid_rule_error_ignored);

        assert(!invalid_rule_error_ignored);
        if (child && child_rules) {
          EmitDeclarationsRuleIfNeeded(context,
              rule_type,
              nesting_type, parent_rule_for_nesting,
              nested_declarations_start_index, *child_rules);
          nested_declarations_start_index = context.parsed_properties.size();
          child_rules->push_back(std::move(child));
        }
        break;
    }

    case CSSParserTokenType::Ident: {
      CSSParserTokenStream::State state = context.token_stream.Save();
      bool consumed_declaration = false;
      {
        // 标记找到下一个;为止。
        CSSParserTokenStream::Boundary boundary(context.token_stream,
                                                CSSParserTokenType::Semicolon);
        consumed_declaration = ConsumeDeclaration(context, rule_type);
      }
      if (consumed_declaration) {
        if (!context.token_stream.AtEnd()) {
          // 如果不是真的结束了，那下一个肯定是刚才设置的临时boundary符号 ;
          assert(context.token_stream.Peek().Type() ==
                 CSSParserTokenType::Semicolon);
          context.token_stream.Consume(); // Semicolon
        }
        break;
      }
      if (context.token_stream.Peek().Type() == CSSParserTokenType::Semicolon) {
        // 遇到未知的Property Id。跳过这个Property
        context.token_stream.Consume(); // Semicolon
        break;
      }
      assert(false); // TODO: 什么场景?
      // Retry as nested rule.
      context.token_stream.Restore(state);
      [[fallthrough]];
    }
    default:
      if (nesting_type != CSSNestingType::None &&
          nesting_type != CSSNestingType::Function) {
        bool invalid_rule_error = false;
        A<StyleRuleBase> child =
              ConsumeNestedRule(std::nullopt, rule_type, context, nesting_type,
                                parent_rule_for_nesting, invalid_rule_error);
        if (child) {
            if (child_rules) {
              EmitDeclarationsRuleIfNeeded(
                context,
                  rule_type, nesting_type, parent_rule_for_nesting,
                  nested_declarations_start_index, *child_rules);
              nested_declarations_start_index = context.parsed_properties.size();
              child_rules->push_back(std::move(child));
            }
            break;
        } else if (invalid_rule_error) {
            // https://drafts.csswg.org/css-syntax/#invalid-rule-error
            //
            // This means the rule was valid per the "core" grammar of
            // css-syntax, but the prelude (i.e. selector list) didn't parse.
            // We should not fall through to error recovery in this case,
            // because we should continue parsing immediately after
            // the {}-block.
          break;
        }
      }

    case CSSParserTokenType::Function:
      stream.SkipUntilPeekedTypeIs<CSSParserTokenType::Semicolon>();
      if (!stream.AtEnd()) {
        stream.Consume(); // kSemicolonToken
      }
      break;
    }
    //   context.token_stream.SkipUntilPeekedTypeIs(CSSParserTokenType::Semicolon);
    //   if (!context.token_stream.AtEnd()) {
    //     context.token_stream.Consume(); // Semicolon
    //   }
    //   break;
  }

}

void CSSParser::EmitDeclarationsRuleIfNeeded(
  CSSParserContext& context,
    StyleRule::RuleType rule_type,
    CSSNestingType nesting_type,
    StyleRule* parent_rule_for_nesting,
    size_t start_index,
    std::vector<A<StyleRuleBase>>& child_rules) {
  if (rule_type == StyleRule::kPage) {
    return;
  }
  if (start_index == kNotFound) {
    return;
  }
  if (start_index >= context.parsed_properties.size()) {
    return;
  }

  A<StyleRuleBase> nested_declarations_rule = CreateDeclarationsRule(
      context, nesting_type,
      parent_rule_for_nesting
          ? parent_rule_for_nesting->FirstSelector()
          : nullptr,
      start_index);
  assert(nested_declarations_rule);
  child_rules.push_back(std::move(nested_declarations_rule));

  context.parsed_properties.resize(start_index);
}


A<StyleRuleBase> CSSParser::CreateDeclarationsRule(
  CSSParserContext& context,
    CSSNestingType nesting_type,
    const CSSSelector* selector_list,
    size_t start_index) {
  assert(selector_list || (nesting_type != CSSNestingType::Nesting));
#if 0
  // Create a nested declarations rule containing all declarations from
  // start_index to the end.
  std::vector<CSSPropertyValue> declarations(
      std::span(context.parsed_properties).subspan(start_index));

  // Create the selector for StyleRuleNestedDeclarations's inner StyleRule.

  switch (nesting_type) {
    case CSSNestingType::None:
      break;
    case CSSNestingType::Nesting:
      // For regular nesting, the nested declarations rule should match
      // exactly what the parent rule matches, with top-level specificity
      // behavior. This means the selector list is copied rather than just
      // being referenced with '&'.
      return blink::CreateNestedDeclarationsRule(
          nesting_type, *context_,
          /*selectors=*/CSSSelectorList::Copy(selector_list), declarations);
    case CSSNestingType::kScope:
      // For direct nesting within @scope
      // (e.g. .foo { @scope (...) { color:green } }),
      // the nested declarations rule should match like a :where(:scope) rule.
      //
      // https://github.com/w3c/csswg-drafts/issues/10431
      return blink::CreateNestedDeclarationsRule(
          nesting_type, *context_,
          /*selectors=*/WhereScopeSelector(), declarations);
    case CSSNestingType::kFunction:
      // For descriptors within @function, e.g.:
      //
      //  @function --x() {
      //    --local: 1px;
      //    result: var(--local);
      //  }
      //
      return MakeGarbageCollected<StyleRuleFunctionDeclarations>(
          *CreateCSSPropertyValueSet(declarations, kCSSFunctionDescriptorsMode,
                                     context_->GetDocument()));
  }
#endif
  abort();
}

namespace {

AllowedRules AllowedNestedRules(StyleRule::RuleType parent_rule_type,
                                bool in_nested_style_rule,
                                bool in_mixin) {
  switch (parent_rule_type) {
  case StyleRule::kScope:
    if (!in_nested_style_rule) {
      return kRegularRules;
    }
    [[fallthrough]];
  case StyleRule::kStyle: {
    if (in_mixin) {
      AllowedRules allowed = CSSParser::kNestedGroupRules |
                             AllowedRules{CSSAtRuleID::kCSSAtRuleContents};
      allowed.Remove(CSSAtRuleID::kCSSAtRuleLayer);
      return allowed;
    } else {
      return CSSParser::kNestedGroupRules;
    }
  }
    case StyleRule::kPage:
      return CSSParser::kPageMarginRules;
    case StyleRule::kFunction:
      return CSSParser::kConditionalRules;
    default:
      break;
    }
  return AllowedRules();
}

}  // namespace

A<StyleRuleBase> CSSParser::ConsumeNestedRule(
    std::optional<CSSAtRuleID> id,
    StyleRule::RuleType parent_rule_type,
    CSSParserContext& context,
    CSSNestingType nesting_type,
    StyleRule* parent_rule_for_nesting,
    bool& invalid_rule_error) {
  auto& stream = context.token_stream;
  
  std::vector<CSSPropertyValue> outer_parsed_properties;
  swap(context.parsed_properties, outer_parsed_properties);

  A<StyleRuleBase> child = nullptr;
  AutoReset<bool> reset_in_nested_style_rule(
      &m_in_nested_style_rule,
      m_in_nested_style_rule || parent_rule_type == StyleRule::kStyle);
  if (!id.has_value()) {
    child.reset(ConsumeStyleRule(context, nesting_type, parent_rule_for_nesting, /* nested */ true, invalid_rule_error));
  } else {
    child.reset(ConsumeAtRuleContents(
        *id, stream,
        AllowedNestedRules(parent_rule_type, m_in_nested_style_rule, in_mixin_)
        ));
  }
  context.parsed_properties = std::move(outer_parsed_properties);
  return child;
}

// 可能是普通样式的property，也可能是@rule的discriptor
bool CSSParser::ConsumeDeclaration(CSSParserContext& context, StyleRule::RuleType rule_type) {
  const CSSParserToken& lhs = context.token_stream.ConsumeIncludingWhitespace();
  if (context.token_stream.Peek().Type() != CSSParserTokenType::Colon) {
    return false; // Parse error.
  }
  context.token_stream.Consume(); // Colon

  size_t properties_count = context.parsed_properties.size();

  bool parsing_descriptor = rule_type == StyleRule::kFontFace ||
                            rule_type == StyleRule::kFontPaletteValues ||
                            rule_type == StyleRule::kProperty ||
                            rule_type == StyleRule::kRoute ||
                            rule_type == StyleRule::kCounterStyle ||
                            rule_type == StyleRule::kViewTransition ||
                            rule_type == StyleRule::kFunction;

  uint64_t id = parsing_descriptor
                    ? static_cast<uint64_t>(lhs.ParseAsAtRuleDescriptorID())
                    : static_cast<uint64_t>(lhs.ParseAsUnresolvedCSSPropertyID(
                          /*context_->GetExecutionContext(), context_->Mode()*/));

  bool important = false;
  static_assert(static_cast<uint64_t>(AtRuleDescriptorID::Invalid) == 0u);
  static_assert(static_cast<uint64_t>(CSSPropertyID::Invalid) == 0u);

  context.token_stream.ConsumeWhitespace();

  if (id) {
    if (parsing_descriptor) {
      const AtRuleDescriptorID atrule_id = static_cast<AtRuleDescriptorID>(id);
      const AtomicString& variable_name =
          (atrule_id == AtRuleDescriptorID::Variable ? lhs.Name() : g_null_atom);
      assert(false);
#if 0
      AtRuleDescriptorParser::ParseDescriptorValue(
          rule_type, atrule_id, variable_name, stream, *context_,
          parsed_properties_);
#endif          
    } else {
      CSSPropertyID property_id = CSSPropertyNameToId(lhs.Name());
      if (property_id == CSSPropertyID::Variable) {
        if (rule_type != StyleRule::kStyle && rule_type != StyleRule::kScope &&
            rule_type != StyleRule::kKeyframe) {
          return false;
        }
        AtomicString variable_name = lhs.Name();
        bool allow_important_annotation = (rule_type != StyleRule::kKeyframe);
        bool is_animation_tainted = rule_type == StyleRule::kKeyframe;
        if (!ConsumeVariableValue(context, variable_name,
                                  allow_important_annotation,
                                  is_animation_tainted)) {
          return false;
        }
      } else if (property_id != CSSPropertyID::Invalid) {
        ConsumeDeclarationValue(context, property_id);
      }
    }
  }

  // if (!id) {
  //   A<CSSVariableData> temp = CSSVariableParser::ConsumeUnparsedDeclaration(
  //       context.token_stream, true, false, false, true, false, important);
  // }
  // context.token_stream.SkipUntilPeekedTypeIs<CSSParserTokenType::LeftBrace,
  //                                            CSSParserTokenType::Semicolon>();

  return context.parsed_properties.size() != properties_count;
}

void CSSParser::ConsumeDeclarationValue(CSSParserContext &context,
                                        CSSPropertyID property_id) {

  bool allow_important_annotation = true; // TODO:
  CSSPropertyParser::ParseValue(context, property_id, allow_important_annotation);
}

bool CSSParser::ParseStyleSheet(StyleSheetContents* style_sheet, const char *bytes, size_t size) {
  CSSParserContext context;
  context.token_stream.SetInput(bytes, size);
  context.style_sheet = style_sheet;
  
  return ConsumeRuleList(context, kTopLevelRules, CSSNestingType::None,
      /*parent_rule_for_nesting=*/nullptr);
}

A<StyleRuleBase> CSSParser::ConsumeStyleRule(CSSParserContext &context,
                                             CSSNestingType nesting_type,
                                             StyleRule *parent_rule_for_nesting,
                                             bool nested,
                                             bool &invalid_rule_error) {
  auto &stream = context.token_stream;

  if (!m_in_nested_style_rule) {
    assert(0u == m_arena.size());
  }

  auto func_clear_arena = [&](std::vector<CSSSelector>* arena) {
    if (!m_in_nested_style_rule) {
      arena->resize(0);  // See class comment on CSSSelectorParser.
    }
  };
  std::unique_ptr<std::vector<CSSSelector>, decltype(func_clear_arena)>
      scope_guard(&m_arena, std::move(func_clear_arena));

  bool custom_property_ambiguity =
      CSSVariableParser::StartsCustomPropertyDeclaration(context.token_stream);
  
  bool has_visited_pseudo = false;
  // Parse the prelude of the style rule

  std::vector<CSSSelector> selector_vector = CSSSelectorParser::ConsumeSelector(
      context, nesting_type, parent_rule_for_nesting);

  if (selector_vector.empty()) {
    if (nested) {
      stream.SkipUntilPeekedTypeIs<CSSParserTokenType::LeftBrace, CSSParserTokenType::Semicolon>();
    } else {
      stream.SkipUntilPeekedTypeIs<CSSParserTokenType::LeftBrace>();
    }
  }

  if (stream.Peek().GetType() != CSSParserTokenType::LeftBrace) {
    return nullptr;
  }

  if (custom_property_ambiguity) {
    if (nested) {
      return nullptr;
    }
    CSSParserTokenStream::BlockGuard guard(stream);
    return nullptr;
  }
  
  if (selector_vector.empty()) {
    CSSParserTokenStream::BlockGuard guard(stream);
    invalid_rule_error = true;
    return nullptr;
  }
  CSSParserTokenStream::BlockGuard guard(stream);
  return ConsumeStyleRuleContents(selector_vector, context, has_visited_pseudo);
}

A<StyleRuleBase> CSSParser::ConsumeStyleRuleContents(
    const std::vector<CSSSelector>& selector_vector,
    CSSParserContext &context,
    bool has_visited_pseudo) {
  CSSParserTokenStream& stream = context.token_stream;

  A<StyleRule> style_rule = StyleRule::Create(selector_vector);
  std::vector<A<StyleRuleBase>> child_rules;

  ConsumeBlockContents(context, StyleRule::kStyle, 
    CSSNestingType::Nesting,
    /*parent_rule_for_nesting=*/style_rule.get(),
    /*nested_declarations_start_index=*/kNotFound,
    &child_rules
    /*, has_visited_pseudo*/);
  for (A<StyleRuleBase>& child_rule : child_rules) {
    style_rule->AddChildRule(std::move(child_rule));
  }
  
  style_rule->SetProperties(std::move(context.parsed_properties));
#if 0
  style_rule->SetProperties(CreateCSSPropertyValueSet(
      context.parsed_properties/*, context_->Mode(), context_->GetDocument()*/));
#endif
  return style_rule;
}

// qualified rule包含两种：普通style和关键帧
A<StyleRuleBase> CSSParser::ConsumeQualifiedRule(
    CSSParserContext &context, AllowedRules allowed_rules, CSSNestingType nesting_type,
    StyleRule *parent_rule_for_nesting) {

  // 普通style
  if (allowed_rules.Has(QualifiedRuleType::Style)) {
    bool invalid_rule_error_ignored = false;  // Only relevant when nested.
    return ConsumeStyleRule(context, nesting_type, parent_rule_for_nesting, /* nested */ false, invalid_rule_error_ignored);
  }

  // keyframe
  // @keyframe animate-name { keyframe-selector { property: value; }}
  // keyframe-selector 有两种： from/to,  percentage(0%, 50%, 100%)
  if (allowed_rules.Has(QualifiedRuleType::Keyframe)) {
    assert(false);
    // TODO;
  }
  return nullptr;
}

static AllowedRules ComputeNewAllowedRules(
    AllowedRules old_allowed_rules,
    StyleRuleBase* rule,
    bool& seen_import_or_namespace_rule) {
  if (!rule) {
    return old_allowed_rules;
  }

  AllowedRules new_allowed_rules = old_allowed_rules;
  if (rule->IsCharsetRule()) {
    // @charset is only allowed once.
    new_allowed_rules.Remove(CSSAtRuleID::Charset);
  } else if (rule->IsLayerStatementRule() && !seen_import_or_namespace_rule) {
    // Any number of @layer statements may appear before @import rules.
    new_allowed_rules.Remove(CSSAtRuleID::Charset);
  } else if (rule->IsImportRule()) {
    // @layer statements are still allowed once @import rules have been seen,
    // but they are treated as regular rules ("else" branch).
    seen_import_or_namespace_rule = true;
    new_allowed_rules.Remove(CSSAtRuleID::Charset);
  } else if (rule->IsNamespaceRule()) {
    // @layer statements are still allowed once @namespace rules have been seen,
    // but they are treated as regular rules ("else" branch).
    seen_import_or_namespace_rule = true;
    new_allowed_rules.Remove(CSSAtRuleID::Charset);
    new_allowed_rules.Remove(CSSAtRuleID::Import);
  } else {
    // Any regular rule must come after @charset/@import/@namespace.
    new_allowed_rules.Remove(CSSAtRuleID::Charset);
    new_allowed_rules.Remove(CSSAtRuleID::Import);
    new_allowed_rules.Remove(CSSAtRuleID::Namespace);
  }
  return new_allowed_rules;
}

// 解析<style>或.css文件内容
bool CSSParser::ConsumeRuleList(CSSParserContext &context,
                                AllowedRules allowed_rules,
                                CSSNestingType nesting_type,
                                StyleRule *parent_rule_for_nesting) {
  auto& stream = context.token_stream;
  bool allow_cdo_cdc_tokens = true;

  bool seen_rule = false;
  bool seen_import_or_namespace_rule = false;
  bool first_rule_valid = false;
  while (!stream.AtEnd()) {
    unsigned int offset = stream.Offset();
    A<StyleRuleBase> rule = nullptr;
    switch (stream.Peek().GetType()) {
      case CSSParserTokenType::Whitespace:
        stream.Consume();
        continue;
      case CSSParserTokenType::AtKeyword:
        rule.reset(ConsumeAtRule(context, allowed_rules));
        break;
      case CSSParserTokenType::CDO:
      case CSSParserTokenType::CDC:
        if (allow_cdo_cdc_tokens) {
          stream.Consume();
          continue;
        }
        [[fallthrough]];
      default:
        rule.reset(ConsumeQualifiedRule(context, allowed_rules, nesting_type,
                                    parent_rule_for_nesting));
        break;
    }
    if (!seen_rule) {
      seen_rule = true;
      first_rule_valid = rule;
    }
    if (rule) {
      allowed_rules = ComputeNewAllowedRules(allowed_rules, rule.get(),
                                             seen_import_or_namespace_rule);
      onConsumeRuleList(context, std::move(rule), offset);
    }
    assert(stream.Offset() >= offset);
  }

  return first_rule_valid;
}

void CSSParser::onConsumeRuleList(CSSParserContext &context,
                                  A<StyleRuleBase> &&rule,
                                  unsigned int offset) {
  if (rule->IsCharsetRule()) {
    return;
  }
#if 0
  if (rule->IsImportRule()) {
    if (!allow_import_rules || context->IsForMarkupSanitization()) {
      result = ParseSheetResult::kHasUnallowedImportRule;
      return;
    }
    Document *document = style_sheet->AnyOwnerDocument();
    if (document) {
      TextPosition position = TextPosition::MinimumPosition();
      probe::GetTextPosition(document, offset, &string, &position);
      To<StyleRuleImport>(rule)->SetPositionHint(position);
    }
  }
#endif
  context.style_sheet->ParserAppendRule(std::move(rule));
}

A<StyleRuleBase> CSSParser::ConsumeAtRule(CSSParserContext &context,
                                    AllowedRules allowed_rules) {
  auto &stream = context.token_stream;
  assert(stream.Peek().GetType() == CSSParserTokenType::AtKeyword);

  CSSParserToken name_token = stream.ConsumeIncludingWhitespace();

  const std::u16string &name = name_token.Name();
  const CSSAtRuleID id = CssAtRuleID(name);
  return ConsumeAtRuleContents(id, stream, allowed_rules);
}

A<StyleRuleBase> CSSParser::ConsumeAtRuleContents(
    CSSAtRuleID id,
    CSSParserTokenStream& stream,
    AllowedRules allowed_rules) {
  if (!allowed_rules.Has(id)) {
    ConsumeErroneousAtRule(stream, id);
    return nullptr;
  }

  switch (id) {
    case CSSAtRuleID::Namespace:
      return ConsumeNamespaceRule(stream);
 
    case CSSAtRuleID::Invalid:
    case CSSAtRuleID::Count:
      ConsumeErroneousAtRule(stream, id);
      return nullptr;

    default:
      assert(false);
      return nullptr;
  }
}

// This may still consume tokens if it fails
static AtomicString ConsumeStringOrURI(CSSParserTokenStream& stream) {
  const CSSParserToken& token = stream.Peek();

  if (token.GetType() == CSSParserTokenType::String || token.GetType() == CSSParserTokenType::Url) {
    return stream.ConsumeIncludingWhitespace().Name();
  }

  if (token.GetType() != CSSParserTokenType::Function ||
      !EqualIgnoringASCIICase(token.Name().c_str(), u"url")) {
    return AtomicString();
  }

  AtomicString result;
  {
    CSSParserTokenStream::BlockGuard guard(stream);
    stream.ConsumeWhitespace();
    const CSSParserToken& uri = stream.ConsumeIncludingWhitespace();
    if (uri.GetType() != CSSParserTokenType::BadString && stream.AtEnd()) {
      result = uri.Name();
    }
  }
  stream.ConsumeWhitespace();
  return result;
}
// namespace必须是分号结尾
A<StyleRuleNamespace> CSSParser::ConsumeNamespaceRule(
    CSSParserTokenStream& stream) {
  AtomicString namespace_prefix;
  if (stream.Peek().GetType() == CSSParserTokenType::Ident) {
    namespace_prefix =
        stream.ConsumeIncludingWhitespace().Name();
  }

  AtomicString uri(ConsumeStringOrURI(stream));
  if (uri.IsNull()) {
    // Parse error, expected string or URI.
    ConsumeErroneousAtRule(stream, CSSAtRuleID::Namespace);
    return nullptr;
  }
  if (!ConsumeEndOfPreludeForAtRuleWithoutBlock(
          stream, CSSAtRuleID::Namespace)) {
    return nullptr;
  }

  return A<StyleRuleNamespace>::make_new(namespace_prefix, uri);
}
// @语法以分号结尾
bool CSSParser::ConsumeEndOfPreludeForAtRuleWithoutBlock(
    CSSParserTokenStream& stream,
    CSSAtRuleID id) {
  stream.ConsumeWhitespace();
  if (stream.AtEnd()) {
    return true;
  }
  if (stream.Peek().GetType() == CSSParserTokenType::Semicolon) {
    stream.Consume();  // kSemicolonToken
    return true;
  }

  // Consume the erroneous block.
  ConsumeErroneousAtRule(stream, id);
  return false;  // Parse error, we expected no block.
}

bool CSSParser::ConsumeEndOfPreludeForAtRuleWithBlock(
    CSSParserTokenStream& stream,
    CSSAtRuleID id) {
  stream.ConsumeWhitespace();

  if (stream.AtEnd()) {
    return false;
  }
  if (stream.Peek().GetType() == CSSParserTokenType::LeftBrace) {
    return true;
  }

  // We have a parse error, so we need to return an error, but before that,
  // we need to consume until the end of the declaration.
  ConsumeErroneousAtRule(stream, id);
  return false;
}


void CSSParser::ConsumeErroneousAtRule(CSSParserTokenStream &stream,
                                       CSSAtRuleID id) {
  // Consume the prelude and block if present.
  stream.SkipUntilPeekedTypeIs<CSSParserTokenType::LeftBrace,
                               CSSParserTokenType::Semicolon>();
  if (!stream.AtEnd()) {
    if (stream.Peek().GetType() == CSSParserTokenType::LeftBrace) {
      CSSParserTokenStream::BlockGuard guard(stream);
    } else {
      stream.Consume(); // kSemicolonToken
    }
  }
}

bool CSSParser::ConsumeVariableValue(CSSParserContext& context,
                                         const AtomicString& variable_name,
                                         bool allow_important_annotation,
                                         bool is_animation_tainted) {
  CSSParserTokenStream& stream = context.token_stream;

  // First, see if this is (only) a CSS-wide keyword.
  bool important;
  A<CSSValue> value = CSSPropertyParser::ConsumeCSSWideKeyword(
      context, allow_important_annotation, important);
  if (!value) {
    A<CSSVariableData> variable_data =
        CSSVariableParser::ConsumeUnparsedDeclaration(
            stream, allow_important_annotation, is_animation_tainted,
            /*must_contain_variable_reference=*/false,
            /*restricted_value=*/false, /*comma_ends_declaration=*/false,
            important/*, *context_*/);
    if (!variable_data) {
      return false;
    }

    value.reset(A<CSSUnparsedDeclarationValue>::make_new(
        std::move(variable_data) /*, context_*/));
  }
  context.parsed_properties.push_back(
      CSSPropertyValue(CSSPropertyName(variable_name), std::move(value), important));
  return true;
}

}