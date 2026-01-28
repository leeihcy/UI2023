#include "html/base/atomic_string.h"
#include "html/util/util.h"

namespace html {

static AtomicStringCache g_cache;

const AtomicString g_null_atom = g_cache.Get(nullptr);
const AtomicString g_empty_atom = g_cache.Get(u"");
const AtomicString g_star_atom = g_cache.Get(u"*");

AtomicString::AtomicString(const char* text) {
  std::u16string unicode;
  QuasiAsciiToUnicode(text, unicode);
  m_text = g_cache.Get(unicode).m_text;
}
AtomicString::AtomicString(const char16_t* text):m_text(g_cache.Get(text).m_text) {
}
AtomicString::AtomicString(const std::u16string& text) : m_text(g_cache.Get(text).m_text) {
}

AtomicString AtomicString::Lower() {
  if (IsNull()) {
    return *this;
  }

  if (IsLower(text().c_str())) [[likely]] {
    return *this;
  }

  std::u16string lower_out;
  UnicodeToLower(text(), lower_out);
  return AtomicString(lower_out);
}


}