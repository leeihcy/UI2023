#ifndef _HTML_DOM_SPACD_SPLIT_STRING_H_
#define _HTML_DOM_SPACD_SPLIT_STRING_H_
#include "html/base/atomic_string.h"
#include <vector>

namespace html {

class SpaceSplitString {
public:
  SpaceSplitString() {}
  explicit SpaceSplitString(const AtomicString& string) { Set(string); }

  void Set(const AtomicString& input_string) {
    if (input_string.IsNull()) {
      Clear();
      return;
    }
    // data_ = Data::Create(input_string);
    assert(false); // TODO
  }
  void Clear() { m_vector.clear(); }

  std::vector<AtomicString> m_vector;
};

}

#endif