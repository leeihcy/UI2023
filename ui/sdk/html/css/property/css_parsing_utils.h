#ifndef _UI_SDK_HTLM_CSS_PROPERTY_CSSPARSINGUTILS_H_
#define _UI_SDK_HTLM_CSS_PROPERTY_CSSPARSINGUTILS_H_

#include "html/css/property/property_id.h"
#include "html/css/property/css_value.h"

namespace html {
class CSSValue;
class CSSIdentifierValue;
class CSSParserContext;
class CSSParserTokenStream;

namespace css_parsing_utils {

U<CSSValue> ConsumeCSSWideKeyword(CSSParserTokenStream &);
U<CSSValue> ConsumeColor(CSSParserTokenStream &stream);
U<CSSIdentifierValue> ConsumeIdent(CSSParserTokenStream &stream);

U<CSSValue> ParseLonghand(CSSPropertyId property_id,
                              CSSParserContext &context);

U<CSSPrimitiveValue> ConsumeLengthOrPercent(
    CSSParserContext& context,
    CSSPrimitiveValue::ValueRange value_range
);
bool ConsumePosition(CSSParserContext &context, CSSValue *&result_x,
                     CSSValue *&result_y);

bool ConsumeCommaIncludingWhitespace(CSSParserTokenStream& stream);

bool MaybeConsumeImportant(CSSParserTokenStream &stream,
                           bool allow_important_annotation);

} // namespace css_parsing_utils
} // namespace html
#endif