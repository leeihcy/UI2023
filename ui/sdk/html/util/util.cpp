#include "html/util/util.h"
#include <cctype>

namespace html {

// deepseek生成的实现代码。
bool Utf8ToUnicode(const char* utf8, size_t utf8_length, std::u16string &out_unicode) {
  out_unicode.clear();
  out_unicode.reserve(utf8_length); // 预分配

  auto it = utf8;
  auto end = utf8 + utf8_length;

  while (it != end) {
    unsigned char c = static_cast<unsigned char>(*it);
    char32_t code_point;

    if (c < 0x80) {
      // 1字节
      code_point = c;
      ++it;
    } else if ((c & 0xE0) == 0xC0) {
      // 2字节
      if (end - it < 2)
        return false;
      code_point = ((c & 0x1F) << 6) | (it[1] & 0x3F);
      it += 2;
    } else if ((c & 0xF0) == 0xE0) {
      // 3字节
      if (end - it < 3)
        return false;
      code_point = ((c & 0x0F) << 12) | ((it[1] & 0x3F) << 6) | (it[2] & 0x3F);
      it += 3;
    } else if ((c & 0xF8) == 0xF0) {
      // 4字节
      if (end - it < 4)
        return false;
      code_point = ((c & 0x07) << 18) | ((it[1] & 0x3F) << 12) |
                   ((it[2] & 0x3F) << 6) | (it[3] & 0x3F);
      it += 4;
    } else {
      return false; // 非法字节
    }

    // 验证并转换到UTF-16
    if (code_point <= 0xFFFF) {
      if (code_point >= 0xD800 && code_point <= 0xDFFF) {
        return false; // 孤立的代理区
      }
      out_unicode.push_back(static_cast<char16_t>(code_point));
    } else if (code_point <= 0x10FFFF) {
      code_point -= 0x10000;
      out_unicode.push_back(static_cast<char16_t>(0xD800 | (code_point >> 10)));
      out_unicode.push_back(
          static_cast<char16_t>(0xDC00 | (code_point & 0x3FF)));
    } else {
      return false; // 超出范围
    }
  }

  return true;
}

bool UnicodeToLowerAscii(const std::u16string& unicode, std::string& out_ascii) {
  out_ascii.clear();
  
  int length = unicode.length();
  out_ascii.resize(length);

  for (int i = 0; i < length; i++) {
    char16_t c = unicode[i];
    if (c == 0 || c >= 0x7F) {
      return false;
    }
    out_ascii[i] = (char)std::tolower((char)c);
  }
  return true;
}

inline char16_t ToASCIILower(char16_t ch) {
  if (ch >= u'A' && ch <= u'Z') {
    return ch + (u'a' - u'A');
  }
  return ch;
}

bool EqualIgnoringASCIICase(const char16_t *s1, const char16_t *s2) {
  while (true) {
    char16_t c1 = ToASCIILower(*s1++);
    char16_t c2 = ToASCIILower(*s2++);

    if (c1 != c2) {
      return false;
    }

    if (c1 == u'\0') {
      return true; // 结束
    }
  }
}


} // namespace html