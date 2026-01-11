#ifndef _UI_SDK_HTLM_CSS_PROPERTY_CSSPARSINGUTILS_H_
#define _UI_SDK_HTLM_CSS_PROPERTY_CSSPARSINGUTILS_H_

#include "html/css/parser/css_parser_token.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_value.h"
#include "html/css/property/value_id.h"
#include "html/css/parser/css_parser_token_stream.h"
#include <cstddef>

namespace html {
class CSSValue;
class CSSIdentifierValue;
struct CSSParserContext;
class CSSParserTokenStream;

namespace css_parsing_utils {

A<CSSValue> ConsumeCSSWideKeyword(CSSParserTokenStream &);
A<CSSValue> ConsumeColor(CSSParserTokenStream &stream);
A<CSSIdentifierValue> ConsumeIdent(CSSParserTokenStream &stream);

A<CSSValue> ParseLonghand(CSSPropertyId property_id,
                              CSSParserContext &context);

A<CSSPrimitiveValue> ConsumeLengthOrPercent(
    CSSParserContext& context,
    CSSPrimitiveValue::ValueRange value_range
);
A<CSSValue> GetSingleValueOrMakeList(
    CSSValueListSeparator list_separator,
    std::vector< A<CSSValue> >&& values);
    
bool ConsumePosition(CSSParserContext &context, A<CSSValue>& result_x,
                     A<CSSValue>& result_y);
A<CSSValue> ConsumeBackgroundSize(CSSParserContext &context);
A<CSSValue> ConsumeBackgroundComponent(CSSPropertyId resolved_property,
                                     CSSParserContext& context);
A<CSSRepeatStyleValue> ConsumeRepeatStyleValue(CSSParserTokenStream& stream);

bool ConsumeCommaIncludingWhitespace(CSSParserTokenStream& stream);
bool ConsumeSlashIncludingWhitespace(CSSParserTokenStream& stream);

bool MaybeConsumeImportant(CSSParserTokenStream &stream,
                           bool allow_important_annotation);

template <typename... empty>
inline bool IdentMatches(CSSValueId id) {
  return false;
}
template <CSSValueId head, CSSValueId... tail>
inline bool IdentMatches(CSSValueId id) {
  return id == head || IdentMatches<tail...>(id);
}
template <CSSValueId... allowIndents> 
bool PeekedIdentMatches(CSSParserTokenStream &stream) {
  return stream.Peek().GetType() == CSSParserTokenType::Ident &&
    IdentMatches<allowIndents...>(stream.Peek().ValueId());
}
template <CSSValueId... names>
A<CSSIdentifierValue> ConsumeIdent(CSSParserTokenStream &stream) {
  if (!PeekedIdentMatches<names...>(stream)) {
    return nullptr;
  }
  return CSSIdentifierValue::Create(stream.ConsumeIncludingWhitespace().ValueId());
}

template <CSSValueId start, CSSValueId end>
A<CSSValue> ConsumePositionLonghand(CSSParserTokenStream &stream, CSSParserContext &context) {
  if (stream.Peek().GetType() == CSSParserTokenType::Ident) {
    CSSValueId id = stream.Peek().ValueId();
    int percent;
    if (id == start) {
      percent = 0;
    } else if (id == CSSValueId::Center) {
      percent = 50;
    } else if (id == end) {
      percent = 100;
    } else {
      return nullptr;
    }
    stream.ConsumeIncludingWhitespace();
    return CSSNumericLiteralValue::Create(
        percent, CSSPrimitiveValue::UnitType::kPercentage);
  }
  return ConsumeLengthOrPercent(context, CSSPrimitiveValue::ValueRange::kAll);
}

} // namespace css_parsing_utils
} // namespace html
#endif