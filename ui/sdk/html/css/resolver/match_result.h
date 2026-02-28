#ifndef _HTML_CSS_RESOLVER_MATCHRESULT_H_
#define _HTML_CSS_RESOLVER_MATCHRESULT_H_

#include "html/css/css_property_value_set.h"
#include "html/css/property/css_value.h"

namespace html {

class MatchedProperties {
public:
  MatchedProperties(CSSPropertyValueSet* p):properties(p) {}

private:
  CSSPropertyValueSet* properties = nullptr;
};

using MatchedPropertiesVector = std::vector<MatchedProperties>;
// using MatchedPropertiesHashVector = std::vector<MatchedPropertiesHash>;

class MatchResult {
public:
   void AddMatchedProperties(
      const CSSPropertyValueSet* properties/*,
      const MixinParameterBindings* mixin_parameter_bindings,
      MatchedProperties::Data types*/);

private:


  MatchedPropertiesVector matched_properties_;
};

}

#endif