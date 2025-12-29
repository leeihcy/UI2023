#ifndef _UI_SDK_HTLM_CSS_PROPERTY_CSSPARSINGUTILS_H_
#define _UI_SDK_HTLM_CSS_PROPERTY_CSSPARSINGUTILS_H_

#include "html/css/property/property_id.h"

namespace html {
class CSSValue;
class CSSIdentifierValue;
class CSSParserContext;
class CSSParserTokenStream;

namespace css_parsing_utils {

CSSValue *ConsumeCSSWideKeyword(CSSParserTokenStream &);
CSSValue *ConsumeColor(CSSParserTokenStream &stream);
CSSIdentifierValue* ConsumeIdent(CSSParserTokenStream& stream);
const CSSValue* ParseLonghand(CSSPropertyId property_id, CSSParserContext& context);

bool MaybeConsumeImportant(CSSParserTokenStream &stream,
                           bool allow_important_annotation);

} // namespace css_parsing_utils
} // namespace html
#endif