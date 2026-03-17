// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file defines utility functions for working with strings.

#ifndef BASE_STRINGS_STRING_UTIL_H_
#define BASE_STRINGS_STRING_UTIL_H_

#include <string>
#include "base/base_export.h"
#include "base/strings/string_util_internal.h"


namespace base {

inline bool EqualsCaseInsensitiveASCII(std::string_view a, std::string_view b) {
  return internal::EqualsCaseInsensitiveASCIIT(a, b);
}
inline bool EqualsCaseInsensitiveASCII(std::u16string_view a,
                                       std::u16string_view b) {
  return internal::EqualsCaseInsensitiveASCIIT(a, b);
}
inline bool EqualsCaseInsensitiveASCII(std::u16string_view a,
                                       std::string_view b) {
  return internal::EqualsCaseInsensitiveASCIIT(a, b);
}
inline bool EqualsCaseInsensitiveASCII(std::string_view a,
                                       std::u16string_view b) {
  return internal::EqualsCaseInsensitiveASCIIT(a, b);
}


// Contains the set of characters representing whitespace in the corresponding
// encoding. Null-terminated. The ASCII versions are the whitespaces as defined
// by HTML5, and don't include control characters.
BASE_EXPORT extern const wchar_t kWhitespaceWide[];    // Includes Unicode.
BASE_EXPORT extern const char16_t kWhitespaceUTF16[];  // Includes Unicode.
BASE_EXPORT extern const char16_t
    kWhitespaceNoCrLfUTF16[];  // Unicode w/o CR/LF.
BASE_EXPORT extern const char kWhitespaceASCII[];
BASE_EXPORT extern const char16_t kWhitespaceASCIIAs16[];  // No unicode.

// https://infra.spec.whatwg.org/#ascii-whitespace
// Note that this array is not null-terminated.
inline constexpr char kInfraAsciiWhitespace[] = {0x09, 0x0A, 0x0C, 0x0D, 0x20};

// Null-terminated string representing the UTF-8 byte order mark.
BASE_EXPORT extern const char kUtf8ByteOrderMark[];



// Determines the type of ASCII character, independent of locale (the C
// library versions will change based on locale).
template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiWhitespace(Char c) {
  // SAFETY: kWhitespaceASCII is a NUL-terminated string.
  for (const char* cur = kWhitespaceASCII; *cur; ++cur) {
    if (*cur == c) {
      return true;
    }
  }
  return false;
}
template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiAlpha(Char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiUpper(Char c) {
  return c >= 'A' && c <= 'Z';
}
template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiLower(Char c) {
  return c >= 'a' && c <= 'z';
}
template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiDigit(Char c) {
  return c >= '0' && c <= '9';
}
template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiAlphaNumeric(Char c) {
  return IsAsciiAlpha(c) || IsAsciiDigit(c);
}
template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiPrintable(Char c) {
  return c >= ' ' && c <= '~';
}

template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiControl(Char c) {
  if constexpr (std::is_signed_v<Char>) {
    if (c < 0) {
      return false;
    }
  }
  return c <= 0x1f || c == 0x7f;
}

template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsUnicodeControl(Char c) {
  return IsAsciiControl(c) ||
         // C1 control characters: http://unicode.org/charts/PDF/U0080.pdf
         (c >= 0x80 && c <= 0x9F);
}

template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsAsciiPunctuation(Char c) {
  return c > 0x20 && c < 0x7f && !IsAsciiAlphaNumeric(c);
}

template <typename Char>
  requires(std::integral<Char>)
constexpr bool IsHexDigit(Char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
         (c >= 'a' && c <= 'f');
}

}

#endif