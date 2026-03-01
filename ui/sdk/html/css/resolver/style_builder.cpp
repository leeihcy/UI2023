#include "html/css/resolver/style_builder.h"
#include "html/css/property/css_property.h"
#include "html/css/property/css_value.h"
#include <assert.h>

namespace html {

// void StyleBuilder::ApplyProperty(const CSSPropertyName &name,
//                                  StyleResolverState &state,
//                                  const ScopedCSSValue &scoped_value) {
//   CSSPropertyRef ref(name, state.GetDocument());
//   DCHECK(ref.IsValid());

//   ApplyProperty(ref.GetProperty(), state, scoped_value);
// }

void StyleBuilder::ApplyProperty(const CSSProperty &property,
                                 StyleResolverState &state,
                                 const CSSValue &value) {

  CSSPropertyID id = property.PropertyID();
  // const CSSValue &value = scoped_value.GetCSSValue();
  assert(!property.IsShorthand());

  bool is_inherit = false;
  bool is_initial = false;

  // bool is_inherit = state.ParentNode() && value.IsInheritedValue();
  // bool is_initial = value.IsInitialValue() ||
  //                   (!state.ParentNode() && value.IsInheritedValue());
  // bool is_inherited_for_unset = state.IsInheritedForUnset(property);
  // if (is_inherit && !is_inherited_for_unset) {
  //   state.Style()->SetHasExplicitInheritance();
  //   state.ParentStyle()->SetChildHasExplicitInheritance();
  // } else if (value.IsUnsetValue()) {
  //   assert(!is_inherit && !is_initial);
  //   if (is_inherited_for_unset)
  //     is_inherit = true;
  //   else
  //     is_initial = true;
  // }

  if (is_initial)
    To<Longhand>(property).ApplyInitial(state);
  else if (is_inherit)
    To<Longhand>(property).ApplyInherit(state);
  else
    To<Longhand>(property).ApplyValue(state, value);
}

} // namespace html