#ifndef BASE_STRINGS_STRCAT_H_
#define BASE_STRINGS_STRCAT_H_

#include <string>
#include <span>

namespace base {

std::string StrCat(std::span<const std::string_view> pieces);
std::string StrCat(std::span<const std::string> pieces);

inline std::string StrCat(std::initializer_list<std::string_view> pieces) {
  return StrCat(std::span(pieces));
}

}

#endif