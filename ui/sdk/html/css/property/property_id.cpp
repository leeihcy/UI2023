#include "html/css/property/property_id.h"
#include "html/util/util.h"

namespace html {

CSSPropertyID CSSPropertyNameToId(const std::u16string& name) {
  int length = name.length();

  if (length == 0) {
    return CSSPropertyID::Invalid;
  }
  if (length >= 3 && name[0] == '-' && name[1] == '-') {
    return CSSPropertyID::Variable;
  }
  if (length > MaxCSSPropertyNameLength) {
    return CSSPropertyID::Invalid;
  }
  std::string ascii_name;
  if (!UnicodeToLowerAscii(name, ascii_name)) {
    return CSSPropertyID::Invalid;
  }

  return CSSPropertyIDNameToIdByHash(ascii_name.c_str(), ascii_name.length());
}

}