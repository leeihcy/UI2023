#include "html/css/parser/css_parser.h"

#include "html/css/parser/allowed_rules.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_property_parser.h"
#include "html/css/property/property_id.h"
#include "html/css/property/value_id.h"
#include "html/util/util.h"
#include <assert.h>


namespace html {

// 解析style=""内容
std::unique_ptr<CSSPropertyValueSet>
CSSParser::ParseInlineStyleDeclaration(const char *bytes, size_t size) {
  CSSParserContext context;
  context.token_stream.SetInput(bytes, size);

  ConsumeBlockContents(context);
  return std::make_unique<CSSPropertyValueSet>(
      std::move(context.parsed_properties));
}

// 解析 { } 里的内容。
void CSSParser::ConsumeBlockContents(CSSParserContext &context) {
  while (true) {
    if (context.token_stream.AtEnd()) {
      break;
    }

    switch (context.token_stream.Peek().Type()) {
    case CSSParserTokenType::Whitespace:
    case CSSParserTokenType::Semicolon:
      context.token_stream.Consume();
      break;

    case CSSParserTokenType::AtKeyword:
      assert(false);
      // TODO:
      break;

    case CSSParserTokenType::Ident: {
      CSSParserTokenStream::State state = context.token_stream.Save();
      bool consumed_declaration = false;
      {
        // 标记找到下一个;为止。
        CSSParserTokenStream::Boundary boundary(context.token_stream,
                                                CSSParserTokenType::Semicolon);
        consumed_declaration = ConsumeDeclaration(context);
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
        assert(false); // TODO: 什么场景?
        context.token_stream.Consume(); // Semicolon
        break;
      }
      assert(false); // TODO: 什么场景?
      // Retry as nested rule.
      context.token_stream.Restore(state);
      [[fallthrough]];
    }
    default:
      context.token_stream.SkipUntilPeekedTypeIs(CSSParserTokenType::Semicolon);
      if (!context.token_stream.AtEnd()) {
        context.token_stream.Consume(); // Semicolon
      }
      break;
    }
  }
}

bool CSSParser::ConsumeDeclaration(CSSParserContext& context) {
  CSSParserToken name = context.token_stream.Consume();
  context.token_stream.ConsumeWhitespace();

  if (context.token_stream.Peek().Type() != CSSParserTokenType::Colon) {
    return false;
  }

  context.token_stream.Consume(); // Colon
  context.token_stream.ConsumeWhitespace();

  CSSPropertyId id = CSSPropertyNameToId(name.Name());
  if (id == CSSPropertyId::Variable) {
    assert(false);
    // TODO:
  } else if (id != CSSPropertyId::Invalid) {
    ConsumeDeclarationValue(context, id);
  }
  return true;
}

void CSSParser::ConsumeDeclarationValue(CSSParserContext &context,
                                        CSSPropertyId property_id) {

  bool allow_important_annotation = true; // TODO:
  CSSPropertyParser::ParseValue(context, property_id, allow_important_annotation);
}

bool CSSParser::ParseStyleSheet(const char *bytes, size_t size) {
  CSSParserContext context;
  context.token_stream.SetInput(bytes, size);
  
  return ConsumeRuleList(context, kTopLevelRules);
}

A<StyleRule> CSSParser::ConsumeStyleRule(CSSParserContext &context) {
  // CSSSelectorParser::ConsumeSelector(context);
  return nullptr;
}

// qualified rule包含两种：普通style和关键帧
A<StyleRule> CSSParser::ConsumeQualifiedRule(CSSParserContext &context,
                                             AllowedRules allowed_rules) {

  // 普通style
  if (allowed_rules.Has(QualifiedRuleType::Style)) {
    return ConsumeStyleRule(context);
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

// 解析<style>或.css文件内容
bool CSSParser::ConsumeRuleList(CSSParserContext& context, AllowedRules allowed_rules) {
  auto& stream = context.token_stream;
  bool allow_cdo_cdc_tokens = true;

  bool seen_rule = false;
  bool seen_import_or_namespace_rule = false;
  bool first_rule_valid = false;
  while (!stream.AtEnd()) {
    unsigned int offset = stream.Offset();
    A<StyleRule> rule = nullptr;
    switch (stream.Peek().GetType()) {
      case CSSParserTokenType::Whitespace:
        stream.Consume();
        continue;
      case CSSParserTokenType::AtKeyword:
        assert(false);
        // rule = ConsumeAtRule(context, allowed_rules, nesting_type,
        //                      parent_rule_for_nesting);
        break;
      case CSSParserTokenType::CDO:
      case CSSParserTokenType::CDC:
        if (allow_cdo_cdc_tokens) {
          stream.Consume();
          continue;
        }
        [[fallthrough]];
      default:
        rule.reset(ConsumeQualifiedRule(context, allowed_rules/*, nesting_type,
                                    parent_rule_for_nesting*/));
        break;
    }
    if (!seen_rule) {
      seen_rule = true;
      first_rule_valid = rule;
    }
    assert(false);
#if 0
    if (rule) {
      allowed_rules = ComputeNewAllowedRules(allowed_rules, rule,
                                             seen_import_or_namespace_rule);
      callback(rule, offset);
    }
#endif
    // DCHECK_GT(stream.Offset(), offset);
  }

  return first_rule_valid;
}

}