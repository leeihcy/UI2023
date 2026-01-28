#include "html/css/parser/at_rule_descriptor_id.h"
#include "html/util/util.h"

namespace html {

AtRuleDescriptorID AtRuleDescriptorNameToId(const std::u16string& name) {
  size_t length = name.length();

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
  if (!QuasiUnicodeToLowerAscii(name, ascii_name)) {
    return AtRuleDescriptorID::Invalid;
  }

  return ResolveAtRuleDescriptorID(ascii_name.c_str(), (unsigned int)ascii_name.length());
}
}