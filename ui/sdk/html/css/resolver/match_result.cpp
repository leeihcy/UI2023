#include "html/css/resolver/match_result.h"

namespace html {

void MatchResult::AddMatchedProperties(
      const CSSPropertyValueSet* properties/*,
      const MixinParameterBindings* mixin_parameter_bindings,
      MatchedProperties::Data types*/) {
  matched_properties_.emplace_back(const_cast<CSSPropertyValueSet*>(properties)/*,
                                   mixin_parameter_bindings, data*/);
}
}