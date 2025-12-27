#include "html/css/property/value_id.h"
#include "html/util/util.h"


namespace html {

CSSValueId CSSValueNameToId(const std::u16string& name) {
  int length = name.length();

  if (length == 0) {
    return CSSValueId::Invalid;
  }
  if (length > MaxCSSValueNameLength) {
    return CSSValueId::Invalid;
  }
  std::string ascii_name;
  if (!UnicodeToLowerAscii(name, ascii_name)) {
    return CSSValueId::Invalid;
  }

  return CSSValueNameToIdByHash(ascii_name.c_str(), ascii_name.length());
}


}