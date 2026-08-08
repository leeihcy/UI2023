#ifndef _BASE_LOG_H_
#define _BASE_LOG_H_

#define WIN32_NO_STATUS 
#include <cstdio>
#include <assert.h>

namespace base {

inline void Log(const char* text) {
  printf(text);
  printf("\n");
}
inline void Assert(bool b) {
  if (!b) {
    assert(false);
  }
}

}

#endif