#ifndef _UI_SDK_COMMON_GUID_H_
#define _UI_SDK_COMMON_GUID_H_

namespace ui {

struct Guid {
  unsigned int Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char Data4[8];

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
      return c - 'A';
    }
    if (c >= 'a' && c <= 'f') {
      return c - 'a';
    }
    return 0;
  }
  static Guid build(const char guid_string[37]) {
    Guid guid;
    for (int i = 0; i < 8; ++i) {
      guid.Data1 = (guid.Data1 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[8] == '-');
    for (int i = 9; i < 13; ++i) {
      guid.Data2 = (guid.Data2 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[13] == '-');
    for (int i = 14; i < 18; ++i) {
      guid.Data3 = (guid.Data3 << 4) | parse_hex(guid_string[i]);
    }
    assert(guid_string[18] == '-');
    assert(guid_string[23] == '-');
    // TODO:
    return guid;
  }
};
static Guid GUID_NULL = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

#define __guid(Class) Class::__GUID()

#define DEFINE_CLASS_GUID(guid_string)                                         \
  static const Guid &__GUID() {                                                \
    static Guid s = Guid::build(guid_string);                                               \
    return s;                                                                  \
  }

} // namespace ui

#endif