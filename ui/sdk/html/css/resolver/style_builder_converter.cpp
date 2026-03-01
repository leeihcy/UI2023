#include "html/css/resolver/style_builder_converter.h"
#include "html/css/property/css_value.h"
#include "html/css/resolver/style_resolver_state.h"

namespace html {


StyleColor StyleBuilderConverter::ConvertStyleColor(StyleResolverState& state,
                                                    const CSSValue& value,
                                                    bool for_visited_link) {
  // auto* identifier_value = DynamicTo<CSSIdentifierValue>(value);
  // if (identifier_value) {
  //   CSSValueID value_id = identifier_value->GetValueID();
  //   if (value_id == CSSValueID::kCurrentcolor)
  //     return StyleColor::CurrentColor();
  //   if (StyleColor::IsSystemColorIncludingDeprecated(value_id)) {
  //     return StyleColor(
  //         state.GetDocument().GetTextLinkColors().ColorFromCSSValue(
  //             value, Color(), state.Style()->UsedColorScheme(),
  //             for_visited_link),
  //         value_id);
  //   }
  // }

//   return StyleColor(state.GetDocument().GetTextLinkColors().ColorFromCSSValue(
//       value, Color(), state.Style()->UsedColorScheme(), for_visited_link));
  return StyleColor();
}


}