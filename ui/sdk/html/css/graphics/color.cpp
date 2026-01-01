#include "html/css/graphics/color.h"

namespace html {

namespace {
inline bool IsASCIIDigit(char c) {
  return c >= '0' && c <= '9';
}
inline bool IsASCIIDigit(char16_t c) {
  return c >= '0' && c <= '9';
}
inline bool IsASCIIHexDigit(char c) {
  return IsASCIIDigit(c) || ((c | 0x20) >= 'a' && (c | 0x20) <= 'f');
}
inline bool IsASCIIHexDigit(char16_t c) {
  return IsASCIIDigit(c) || ((c | 0x20) >= 'a' && (c | 0x20) <= 'f');
}
inline int ToASCIIHexValue(char c) {
  return c < 'A' ? c - '0' : (c - 'A' + 10) & 0xF;
}
inline int ToASCIIHexValue(char16_t c) {
  return c < 'A' ? c - '0' : (c - 'A' + 10) & 0xF;
}
}

const Color Color::kBlack = Color(0xFF000000);
const Color Color::kWhite = Color(0xFFFFFFFF);
const Color Color::kDarkGray = Color(0xFF808080);
const Color Color::kGray = Color(0xFFA0A0A0);
const Color Color::kLightGray = Color(0xFFC0C0C0);
const Color Color::kTransparent = Color(0x00000000);

inline bool ParseHexColorInternal(const std::u16string& name,
                                  Color& color) {
  if (name.size() != 3 && name.size() != 4 && name.size() != 6 &&
      name.size() != 8) {
    return false;
  }
  // if ((name.size() == 8 || name.size() == 4) &&
  //     !RuntimeEnabledFeatures::CSSHexAlphaColorEnabled()) {
  //   return false;
  // }
  uint32_t value = 0;
  for (unsigned i = 0; i < name.size(); ++i) {
    if (!IsASCIIHexDigit(name[i]))
      return false;
    value <<= 4;
    value |= ToASCIIHexValue(name[i]);
  }
  if (name.size() == 6) {
    color = Color::FromRGBA32(0xFF000000 | value);
    return true;
  }
  if (name.size() == 8) {
    // We parsed the values into RGBA order, but the RGBA32 type
    // expects them to be in ARGB order, so we right rotate eight bits.
    color = Color::FromRGBA32(value << 24 | value >> 8);
    return true;
  }
  if (name.size() == 4) {
    // #abcd converts to ddaabbcc in RGBA32.
    color = Color::FromRGBA32((value & 0xF) << 28 | (value & 0xF) << 24 |
                              (value & 0xF000) << 8 | (value & 0xF000) << 4 |
                              (value & 0xF00) << 4 | (value & 0xF00) |
                              (value & 0xF0) | (value & 0xF0) >> 4);
    return true;
  }
  // #abc converts to #aabbcc
  color = Color::FromRGBA32(0xFF000000 | (value & 0xF00) << 12 |
                            (value & 0xF00) << 8 | (value & 0xF0) << 8 |
                            (value & 0xF0) << 4 | (value & 0xF) << 4 |
                            (value & 0xF));
  return true;
}

bool Color::ParseHexColor(const std::u16string& name, Color& out) {
  if (name.empty())
    return false;
  return ParseHexColorInternal(name, out);
}

}