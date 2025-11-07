#include "src/util.h"
#include <stdarg.h>
#include <string>
#include <iostream>
#include <assert.h>

namespace ui {

void Log(const char* format, ...) {
  std::string output;
  va_list argList;
  va_start(argList, format);
  {
    const int buffer_size = 4096;
    static char buffer[buffer_size];

    int ret = vsnprintf(buffer, buffer_size, format, argList);
    if (ret < 0 || ret >= buffer_size) {
      assert(false && "Log format too long, please increase max_size");
    }
    output.append(buffer);
  }
  va_end(argList);
  std::cout << output << std::endl;
}

}