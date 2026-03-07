#include "html/css/resolver/style_cascade.h"
#include "html/css/resolver/cascade_resolver.h"
#include "html/css/property/css_property.h"
#include "html/css/resolver/style_builder.h"

namespace html {

void StyleCascade::Apply(CascadeFilter filter) {
  CascadeResolver resolver(filter, ++generation_);

  ApplyMatchResult(resolver);
}

void StyleCascade::ApplyMatchResult(CascadeResolver& resolver) {
  // TODO: priority

   for (const MatchedProperties& properties :
      m_match_result.GetMatchedProperties()) {

    // if (resolver.Rejects(property)) {
    //   continue;
    // }
    for (unsigned int i = 0; i < properties.properties->Count(); i++) {
      CSSPropertyValue& property_value = properties.properties->Item(i);
    
      CascadePriority temp_todo;
      LookupAndApplyDeclaration(property_value, &temp_todo, resolver);
    }
  }
}
void StyleCascade::LookupAndApplyDeclaration(CSSPropertyValue& property_value,
                                             CascadePriority* priority,
                                             CascadeResolver& resolver) {
  const CSSProperty& property = CSSProperty::Get(property_value.Id());

  StyleBuilder::ApplyProperty(property, m_state, *property_value.Value());
  return;
}
}