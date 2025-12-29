#include "html/css/parser/css_parser.h"

#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_property_parser.h"
#include "html/css/property/property_id.h"
#include "html/css/property/value_id.h"
#include "html/util/util.h"
#include <assert.h>


namespace html {

void CSSParser::ParseInlineStyleDeclaration(const char* bytes, size_t size) {
  CSSParserContext context;
  context.token_stream.SetInput(bytes, size);

  ConsumeBlockContents(context);
}

// 解析 { } 里的内容。
void CSSParser::ConsumeBlockContents(CSSParserContext& context) {
  while (true) {
    if (context.token_stream.AtEnd()) {
      break;
    }

    switch (context.token_stream.Peek().Type()) {
    case CSSParserTokenType::Ident:
      if (ConsumeDeclaration(context)) {

      }
    default:
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