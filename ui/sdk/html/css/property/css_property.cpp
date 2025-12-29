#include "html/css/property/css_property.h"
#include "html/css/parser/css_parser.h"
#include "html/css/property/property_id.h"
#include "html/css/property/css_parsing_utils.h"
#include "html/css/property/css_property_instances.h"

#include <assert.h>


namespace html {

// static 
const CSSProperty& CSSProperty::Get(CSSPropertyId id) {
  return GetCSSPropertyInternal(id);
}

const CSSValue * BackgroundColor::ParseSingleValue(CSSParserContext &context) const {
  return css_parsing_utils::ConsumeColor(context.token_stream);
}


}