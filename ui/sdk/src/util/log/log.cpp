#include "include/util/log.h"
#include "include/inc.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#if defined(OS_LINUX)
#include <stdarg.h>
#endif
#include "src/util/windows.h"

// 是否输出文件路径和函数名
static bool g_output_file_function = false;

namespace ui {

const wchar_t *LevelToString(const ui::LOG_LEVEL &l) {
  switch (l) {
  case ui::LOG_LEVEL_DEBUG:
    return L" [DEBUG]   ";
  case ui::LOG_LEVEL_INFO:
    return L" [INFO]    ";
  case ui::LOG_LEVEL_WARN:
    return L" [WARN]    ";
  case ui::LOG_LEVEL_ERROR:
    return L" [ERROR]   ";
  case ui::LOG_LEVEL_FATAL:
    return L" [FATAL]   ";
  default:
    return L" [UNKNOWN] ";
  }
}
const char *LevelToStringA(const ui::LOG_LEVEL &l) {
  switch (l) {
  case ui::LOG_LEVEL_DEBUG:
    return " [DEBUG]   ";
  case ui::LOG_LEVEL_INFO:
    return " [INFO]    ";
  case ui::LOG_LEVEL_WARN:
    return " [WARN]    ";
  case ui::LOG_LEVEL_ERROR:
    return " [ERROR]   ";
  case ui::LOG_LEVEL_FATAL:
    return " [FATAL]   ";
  default:
    return " [UNKNOWN] ";
  }
}

void CDECL Log(ui::LOG_LEVEL lLevel, const char *szFile,
                 const char *szFunction, long lLine, const wchar_t *szFormat,
                 ...) {
  if (lLevel == ui::LOG_LEVEL_DEBUG) {
#if !defined(_DEBUG)
    return;
#endif
  }

  // level
  std::string output;
  output.append(LevelToStringA(lLevel));

  // content
  va_list argList;
  va_start(argList, szFormat);
  {
    // 只接受256长度，不再动态分配内存。如果有需要，再调大这个值
    const int buffer_size = 256;
    static wchar_t buffer[buffer_size];
    static char utf8_buffer[buffer_size*4];

    // 直接在栈上分配
    int ret = vswprintf(buffer, buffer_size, szFormat, argList);
    if (ret < 0 || ret >= buffer_size) {
      assert(false && "Log format too long, please increase max_size");
    }

    // linux不支持同时输出宽窄字符（printf/wprintf）,谁先调用谁抢得输出。
    // 因此统一转换成utf8字符输出。
    wcstombs(utf8_buffer, buffer, buffer_size*4);

    output.append(utf8_buffer);
  }

  va_end(argList);
  output.append("\r\n");

  if (g_output_file_function) {
    // file name, line function
    char buffer[1024] = {0};
    snprintf(buffer, 1024, "\t%s(%ld) : %s\r\n", szFile, lLine, szFunction);
    output.append(buffer);
  }

#if 0 // defined(OS_WIN)
  OutputDebugString(output.c_str());
#else
  printf("%s", output.c_str());
#endif
}

void CDECL Log(ui::LOG_LEVEL lLevel, const char *szFile,
                 const char *szFunction, long lLine, const char *szFormat,
                 ...) {
  if (lLevel == ui::LOG_LEVEL_DEBUG) {
#if !defined(_DEBUG)
    return;
#endif
  }

  // level
  std::string output;
  output.append(LevelToStringA(lLevel));

  // content
  // va_list 只能用一次，循环里要用 va_copy，否则多次 vsnprintf 结果不可靠。
  va_list argList;
  va_start(argList, szFormat);
  {
    // 只接受256长度，不再动态分配内存。如果有需要，再调大这个值
    const int buffer_size = 256;
    static char buffer[buffer_size];

    int ret = vsnprintf(buffer, buffer_size, szFormat, argList);
    if (ret < 0 || ret >= buffer_size) {
      assert(false && "Log format too long, please increase max_size");
    }
    output.append(buffer);
  }
  va_end(argList);
  output.append("\r\n");

  if (g_output_file_function) {
    // file name, line function
    char buffer[1024] = {0};
    snprintf(buffer, 1024, "\t%s(%ld) : %s\r\n", szFile, lLine, szFunction);
    output.append(buffer);
  }

#if 0 // defined(OS_WIN)
  OutputDebugStringA(output.c_str());
#else
  printf("%s", output.c_str());
#endif
}

} // namespace ui