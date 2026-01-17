#ifndef _HTML_BASE_ATOMICSTRING_H_
#define _HTML_BASE_ATOMICSTRING_H_

#include <memory>
#include <string>
#include <assert.h>

namespace html {

// 简易版实现

class AtomicString {
public:
  AtomicString() {}
  AtomicString(const char16_t* text) {
    if (text) {
      m_text = std::make_shared<std::u16string>(text);
    }
  }

  bool IsNull() const {
    return !m_text;
  }
  operator bool() const {
    return !!m_text;
  }
  AtomicString& operator=(const std::u16string& text) {
    m_text = std::make_shared<std::u16string>(text);
    return *this;
  }
  AtomicString& operator=(const char16_t* text) {
    if (!text) {
      m_text = nullptr;
    } else {
      m_text = std::make_shared<std::u16string>(text);
    }
    return *this;
  }

  const char16_t* data() const { 
    if (!m_text) { return nullptr;}
    return m_text->c_str();
  }
  const std::u16string& text() const{
    assert(!IsNull());
    return *m_text;
  }
private:
  std::shared_ptr<std::u16string> m_text;
};

inline bool operator==(const AtomicString& o1, const AtomicString& o2) {
  if (o1.data() == o2.data()) {
    return true;
  }
  if (o1.IsNull() || o2.IsNull()) {
    return false;
  }
  return o1.text() == o2.text();
}

extern const AtomicString g_null_atom;
extern const AtomicString g_empty_atom;
extern const AtomicString g_star_atom;

}

#endif