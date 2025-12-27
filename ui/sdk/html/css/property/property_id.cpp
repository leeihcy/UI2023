#include "html/css/property/property_id.h"
#include "html/util/util.h"

namespace html {

CSSPropertyId CSSPropertyNameToId(const std::u16string& name) {
  int length = name.length();

  if (length == 0) {
    return CSSPropertyId::Invalid;
  }
  if (length >= 3 && name[0] == '-' && name[1] == '-') {
    return CSSPropertyId::Variable;
  }
  if (length > MaxCSSPropertyNameLength) {
    return CSSPropertyId::Invalid;
  }
  std::string ascii_name;
  if (!UnicodeToLowerAscii(name, ascii_name)) {
    return CSSPropertyId::Invalid;
  }

  return CSSPropertyNameToIdByHash(ascii_name.c_str(), ascii_name.length());
}

}