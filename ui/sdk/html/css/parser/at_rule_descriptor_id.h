#ifndef _HTML_CSS_PARSER_CSSATRULEDESCRIPTORS_H_
#define _HTML_CSS_PARSER_CSSATRULEDESCRIPTORS_H_

#include "html/css/parser/at_rule_descriptor_id_enum.h"
#include <string>

namespace html {

const int numAtRuleDescriptors = 42;
const int MaxAtRuleDescriptorNameLength = 24;

AtRuleDescriptorID AtRuleDescriptorNameToId(const std::u16string& name);
AtRuleDescriptorID AtRuleDescriptorIDNameToIdByHash(const char* name, unsigned int len);

}

#endif