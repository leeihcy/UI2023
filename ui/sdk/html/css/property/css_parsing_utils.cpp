#include "html/css/property/css_parsing_utils.h"

#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/parser/css_parser.h"
#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_property.h"
#include "html/css/property/value_id.h"
#include "html/util/util.h"
#include <_types/_uint64_t.h>



namespace html {
namespace css_parsing_utils {

bool IsCSSWideKeyword(CSSValueId id) {
  return id == CSSValueId::Inherit || id == CSSValueId::Initial ||
         id == CSSValueId::Unset || id == CSSValueId::Revert ||
         id == CSSValueId::RevertLayer;
}

CSSValue* ConsumeCSSWideKeyword(CSSParserTokenStream& stream) {
  if (!IsCSSWideKeyword(stream.Peek().ValueId())) {
    return nullptr;
  }
  switch (stream.ConsumeIncludingWhitespace().ValueId()) {
    case CSSValueId::Initial:
      return CSSInitialValue::Create();
    case CSSValueId::Inherit:
      return CSSInheritedValue::Create();
    case CSSValueId::Unset:
      return CSSUnsetValue::Create();
    case CSSValueId::Revert:
      return CSSRevertValue::Create();
    case CSSValueId::RevertLayer:
      return CSSRevertLayerValue::Create();
    // case CSSValueId::RevertRule: {
    //   if (!RuntimeEnabledFeatures::CSSRevertRuleEnabled()) {
    //     return nullptr;
    //   }
    //   return cssvalue::CSSRevertRuleValue::Create();
    // }
    default:
      assert(false);
      abort();
  }
}

bool MaybeConsumeImportant(CSSParserTokenStream& stream,
                           bool allow_important_annotation) {
  stream.ConsumeWhitespace();
  if (stream.AtEnd() || !allow_important_annotation) {
    return false;
  }

  CSSParserSavePoint savepoint(stream);

  // !
  if (stream.Peek().Type() != CSSParserTokenType::Delimiter ||
      stream.Peek().Delimiter() != '!') {
    return false;
  }
  stream.ConsumeIncludingWhitespace();

  // important
  if (stream.Peek().Type() != CSSParserTokenType::Ident ||
      !EqualIgnoringASCIICase(stream.Peek().Name().c_str(), u"important")) {
    return false;
  }
  stream.ConsumeIncludingWhitespace();

  savepoint.Release();
  return true;
}

const CSSValue* ParseLonghand(CSSPropertyId property_id, CSSParserContext& context) {
  // CSSValueId value_id = stream.Peek().ValueId();

  const CSSProperty& property = CSSProperty::Get(property_id);
  return static_cast<const Longhand&>(property).ParseSingleValue(context);
}

CSSIdentifierValue* ConsumeIdent(CSSParserTokenStream& stream) {
  if (stream.Peek().Type() != CSSParserTokenType::Ident) {
    return nullptr;
  }
  return CSSIdentifierValue::Create(stream.ConsumeIncludingWhitespace().ValueId());
}

CSSValue *ConsumeColor(CSSParserTokenStream &stream) {

  if (stream.Peek().FunctionId() == CSSValueId::ColorMix) {
    // CSSValue* color =
    //     ConsumeColorMixFunction(stream);
    // return color;
    assert(false); 
    return nullptr;
  }

  CSSValueId id = stream.Peek().ValueId();
  if (id >= CSSValueId::ColorBasicStart && id <= CSSValueId::ColorExtendedEnd) {
    // if (!IsValueAllowedInMode(id, context.Mode())) {
    //   return nullptr;
    // }
    // if (color_parser_context.OnlyAbsoluteColorsAllowed() &&
    //     (id == CSSValueID::kCurrentcolor ||
    //      StyleColor::IsSystemColorIncludingDeprecated(id) ||
    //      StyleColor::IsSystemColor(id))) {
    //   return nullptr;
    // }
    CSSIdentifierValue* color = ConsumeIdent(stream);
    return color;
  }

  // Color color = Color::kTransparent;
  // if (ParseHexColor(stream, color, accept_quirky_colors)) {
  //   return cssvalue::CSSColor::Create(color);
  // }

  // Parses the color inputs rgb(), rgba(), hsl(), hsla(), hwb(), lab(),
  // oklab(), lch(), oklch() and color(). https://www.w3.org/TR/css-color-4/
  // ColorFunctionParser parser;
  // if (CSSValue* functional_syntax_color = parser.ConsumeFunctionalSyntaxColor(
  //         stream, context, color_parser_context)) {
  //   return functional_syntax_color;
  // }

  // if (!color_parser_context.OnlyAbsoluteColorsAllowed()) {
  //   return ConsumeLightDark(ConsumeColor, stream, context,
  //                           color_parser_context);
  // }
  return nullptr;
}


}
}