#include "html/css/parser/css_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/property/property_id.h"
#include "html/css/property/value_id.h"
#include "html/util/util.h"


namespace html {

void CSSParser::ParseInlineStyleDeclaration(const char* bytes, size_t size) {
  CSSParserContext context;
  context.token_stream.SetInput(bytes, size);

  ConsumeBlockContents(context);
}

// 解析 { } 里的内容。
void CSSParser::ConsumeBlockContents(CSSParserContext& context) {
  while (true) {
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
  context.token_stream.ConsumeWhiteSpace();

  if (context.token_stream.Peek().Type() != CSSParserTokenType::Colon) {
    return false;
  }

  context.token_stream.Consume(); // Colon
  context.token_stream.ConsumeWhiteSpace();

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
  CSSParserToken value = context.token_stream.Consume();
  CSSValueId id = CSSValueNameToId(value.Name());

  // 将property_id转换成对应的属性类，让属性类去做进一步的属性解析。
}

}