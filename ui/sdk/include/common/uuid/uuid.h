#ifndef _UI_SDK_COMMON_GUID_H_
#define _UI_SDK_COMMON_GUID_H_
#include <assert.h>
#include <string.h>

namespace ui {

struct Uuid {
  unsigned int data1;
  unsigned short data2;
  unsigned short data3;
  unsigned char data4[8];

  static Uuid &Null() {
    static Uuid s = {0};
    return s;
  }
  Uuid() { memset(this, 0, sizeof(Uuid)); }

  Uuid(const char guid_string[37]) { build(guid_string); }

  // 支持初始化列表构造函数：
  // Uuid uuid_null = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
  struct Data4 {
    unsigned char data4[8];
  };
  Uuid(unsigned int d1, unsigned short d2, unsigned short d3, Data4 d4)
      : data1(d1), data2(d2), data3(d3) {
    memcpy(data4, d4.data4, sizeof(Data4));
  }

  bool operator==(const Uuid &o) const {
    return memcmp(this, &o, sizeof(Uuid)) == 0;
  }

  static unsigned char parse_hex(char c) {
    if (c >= '0' && c <= '9') {
      return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
      return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
      return c - 'a' + 10;
    }
    return 0;
  }
  void build(const char guid_string[37]) {
    memset(this, 0, sizeof(Uuid));

    for (int i = 0; i < 8; ++i) {
      data1 = (data1 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[8] == '-');
    for (int i = 9; i < 13; ++i) {
      data2 = (data2 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[13] == '-');
    for (int i = 14; i < 18; ++i) {
      data3 = (data3 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[18] == '-');
    data4[0] = (parse_hex(guid_string[19]) << 4) | parse_hex(guid_string[20]);
    data4[1] = (parse_hex(guid_string[21]) << 4) | parse_hex(guid_string[22]);

    assert(guid_string[23] == '-');
    for (int i = 2, j = 24; i < 8; i++, j += 2) {
      data4[i] =
          (parse_hex(guid_string[j]) << 4) | parse_hex(guid_string[j + 1]);
    }
  }
};

} // namespace ui

#endif