#include "html/css/property/css_property.h"
#include "html/css/property/property_id.h"

#include <assert.h>

namespace html {

constexpr int CSS_COUNT = (int)CSSPropertyId::CSSPropertyCount;
// CSSProperty g_all_css_properties[CSS_COUNT] = {
  // Variable(),
  // Background(),
  // BackgroundColor(),
// };

// static 
// const CSSProperty& CSSProperty::Get(CSSPropertyId id) {
//   int n = (int)id;

//   if (n < 0 && n >= CSS_COUNT) {
//     assert(false);
//     abort();
//   }
  
//   return g_all_css_properties[(int)id];
// }

bool BackgroundColor::Parse(CSSParserContext& c) {
  return false;
}
}