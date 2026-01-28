#include "html/css/property/value_id.h"
#include "html/css/property/value_id_enum.h"
#include "html/util/util.h"


namespace html {

CSSValueID CSSValueNameToId(const std::u16string& name) {
  int length = name.length();

  if (length == 0) {
    return CSSValueID::Invalid;
  }
  if (length > MaxCSSValueNameLength) {
    return CSSValueID::Invalid;
  }
  std::string ascii_name;
  if (!QuasiUnicodeToLowerAscii(name, ascii_name)) {
    return CSSValueID::Invalid;
  }

  return ResolveCSSValueID(ascii_name.c_str(), ascii_name.length());
}


}