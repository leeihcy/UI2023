#ifndef _HTML_BASE_ATOMICSTRING_H_
#define _HTML_BASE_ATOMICSTRING_H_

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <assert.h>

namespace html {

// AtomicString 解决了单个字符串的高效存储和比较问题。

// 简易版实现
class AtomicString {
public:
  AtomicString() {}
  AtomicString(const char* text);
  AtomicString(const char16_t* text);
  AtomicString(const std::u16string& text);
  AtomicString(const AtomicString& o);
  AtomicString(AtomicString&& o);
  AtomicString(std::shared_ptr<std::u16string> text) : m_text(text) {}
  AtomicString& operator=(const AtomicString& o);

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

  AtomicString Lower() const;
  
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
inline bool operator==(const AtomicString& o1, const std::u16string& o2) {
  if (o1.IsNull()) {
    return false;
  }
  return o1.text() == o2;
}
inline bool operator==(const AtomicString& o1, const char16_t* o2) {
  if (o1.IsNull() && o2 == nullptr) {
    return true;
  }
  if (o1.IsNull() || !o2) {
    return false;
  }
  return o1.text() == o2;
}

class AtomicStringCache {
public:
  AtomicStringCache() : m_empty(std::make_shared<std::u16string>(u"")) {
  }

  AtomicString Get(const char16_t* text) {
    if (!text) { return m_null; }
    if (!text[0]) { return m_empty; }

    size_t hash = std::hash<std::u16string>{}(text);

    // 查找已存在的字符串
    auto &bucket = m_map[hash];
    for (AtomicString &i : bucket) {
      if (i == text) {
        return i;
      }
    }
    bucket.push_back(AtomicString(std::make_shared<std::u16string>(text)));
    return bucket.back();

    return Get(std::u16string(text));
  }
  AtomicString Get(const std::u16string &text) {
    return Get(text.c_str());
  }
  void Release() {
    m_map.clear();
  }

private:
  AtomicString m_null;
  AtomicString m_empty;
  std::map<size_t, std::vector<AtomicString>> m_map;
};

extern const AtomicString g_null_atom;
extern const AtomicString g_empty_atom;
extern const AtomicString g_star_atom;
}

#endif