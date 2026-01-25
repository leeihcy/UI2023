#include "html/css/property/css_property.h"
#include "html/css/property/property_id.h"
#include <cassert>
#include <string>
#include <tuple>

namespace {

void test1() {
  const html::CSSProperty& property = 
    html::CSSProperty::Get(html::CSSPropertyID::BackgroundColor);
  
  auto& background_color = static_cast<const html::BackgroundColor&>(property);
  assert(background_color.IsLonghand());
  assert(!background_color.IsShorthand());
}

}

void test_html_css_property() {
  test1();
}
