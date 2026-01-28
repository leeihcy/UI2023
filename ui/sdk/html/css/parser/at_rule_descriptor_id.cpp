#include "html/css/parser/at_rule_descriptor_id.h"
#include "html/util/util.h"

namespace html {

AtRuleDescriptorID AtRuleDescriptorNameToId(const std::u16string& name) {
  int length = name.length();

  if (length == 0) {
    return AtRuleDescriptorID::Invalid;
  }
  if (length >= 3 && name[0] == '-' && name[1] == '-') {
    return AtRuleDescriptorID::Variable;
  }
  if (length > MaxAtRuleDescriptorNameLength) {
    return AtRuleDescriptorID::Invalid;
  }
  std::string ascii_name;
  if (!UnicodeToLowerAscii(name, ascii_name)) {
    return AtRuleDescriptorID::Invalid;
  }

  return AtRuleDescriptorIDMap(ascii_name.c_str(), ascii_name.length());
}
}