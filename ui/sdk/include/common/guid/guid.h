#ifndef _UI_SDK_COMMON_GUID_H_
#define _UI_SDK_COMMON_GUID_H_
#include <string.h>
#include <assert.h>

namespace ui {

struct Guid {
  unsigned int data1;
  unsigned short data2;
  unsigned short data3;
  unsigned char data4[8];

  static Guid &Null() {
    static Guid s = {0};
    return s;
  }

  bool operator==(const Guid &o) const {
    return memcmp(this, &o, sizeof(Guid)) == 0;
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
  static Guid Build(const char guid_string[37]) {
    Guid guid;
    memset(&guid, 0, sizeof(guid));

    for (int i = 0; i < 8; ++i) {
      guid.data1 = (guid.data1 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[8] == '-');
    for (int i = 9; i < 13; ++i) {
      guid.data2 = (guid.data2 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[13] == '-');
    for (int i = 14; i < 18; ++i) {
      guid.data3 = (guid.data3 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[18] == '-');
    guid.data4[0] = (parse_hex(guid_string[19]) << 4) | parse_hex(guid_string[20]);
    guid.data4[1] = (parse_hex(guid_string[21]) << 4) | parse_hex(guid_string[22]);
    
    assert(guid_string[23] == '-');
    for (int i = 2, j = 24; i < 8; i++, j+=2) {
      guid.data4[i] = (parse_hex(guid_string[j]) << 4) | parse_hex(guid_string[j+1]);
    }

    return guid;
  }
};
static Guid GUID_NULL = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

#define __guid(Class) Class::__GUID()

#define UI_DEFINE_CLASS_GUID(guid_string)                                      \
  static const Guid &__GUID() {                                                \
    static Guid s = Guid::Build(guid_string);                                  \
    return s;                                                                  \
  }

} // namespace ui

#endif