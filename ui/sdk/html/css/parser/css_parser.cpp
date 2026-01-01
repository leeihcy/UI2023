#include "html/css/parser/css_parser.h"

#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_property_parser.h"
#include "html/css/property/property_id.h"
#include "html/css/property/value_id.h"
#include "html/util/util.h"
#include <assert.h>


namespace html {

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

}