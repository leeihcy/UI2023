#include "html/css/property/css_property.h"
#include "html/css/property/property_id.h"

namespace html {

constexpr int CSS_COUNT = (int)CSSPropertyId::CSSPropertyCount;
CSSProperty g_all_css_properties[CSS_COUNT] = {
  // Variable(),
  // Background(),
  // BackgroundClip(),
};

// static 
const CSSProperty& CSSProperty::Get(CSSPropertyId id) {

}

bool BackgroundColor::Parse(CSSParserContext& c) {
  return false;
}
}