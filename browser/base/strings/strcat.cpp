
#include "base/strings/strcat.h"

namespace base {

namespace internal {

template <typename CharT, typename StringT>
void StrAppendT(std::basic_string<CharT>& dest, std::span<const StringT> strings) {
  const size_t initial_size = dest.size();
  size_t total_size = initial_size;
  for (const StringT& str : strings) {
    total_size += str.size();
  }

  dest.resize_and_overwrite(total_size, [&](CharT* p, size_t n) {
    // SAFETY: `std::basic_string::resize_and_overwrite` guarantees that the
    // range `[p, p + n]` is valid.
    std::span<CharT> to_overwrite(p, n);
    auto write_it = to_overwrite.begin();

    // The first `initial_size` characters are guaranteed to be the previous
    // contents of `dest`.
    write_it += static_cast<std::ptrdiff_t>(initial_size);

    // Copy each string into the destination, resetting `write_it` as we go.
    for (const StringT& str : strings) {
      write_it = std::ranges::copy(str, write_it).out;
    }
    return n;
  });
}

template <typename StringT> auto StrCatT(std::span<const StringT> pieces) {
  std::basic_string<typename StringT::value_type> result;
  StrAppendT(result, pieces);
  return result;
}

} // namespace internal

std::string StrCat(std::span<const std::string_view> pieces) {
  return internal::StrCatT(pieces);
}

std::string StrCat(std::span<const std::string> pieces) {
  return internal::StrCatT(pieces);
}

} // namespace base