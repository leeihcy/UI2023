#include "include/util/log.h"
#include "include/inc.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#if defined(OS_LINUX)
#include <stdarg.h>
#endif
#include "src/util/windows.h"

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
// #if !defined(_DEBUG)
    return;
// #endif
  }

  // level
  std::string output;
  output.append(LevelToStringA(lLevel));

  // content
  va_list argList;
  va_start(argList, szFormat);

  const int max_size = 10240;
  for (int size = 64; size < max_size; size <<= 2) {
    // 直接在栈上分配
    wchar_t *buffer = nullptr;
    buffer = (wchar_t *)/*alloca*/ malloc(size * sizeof(wchar_t));
    int ret = vswprintf(buffer, size, szFormat, argList);
    if (ret < 0 || ret >= size) {
      free(buffer);
      continue;
    }

    // linux不支持同时输出宽窄字符（printf/wprintf）,谁先调用谁抢得输出。
    // 因此统一转换成utf8字符输出。
    int wide_size = wcstombs(nullptr, buffer, 0);
    char *utf8_buffer = nullptr;
    utf8_buffer = (char*) malloc(size+1);
    wcstombs(utf8_buffer, buffer, wide_size);

    output.append(utf8_buffer);
    free(buffer);
    free(utf8_buffer);
    break;
  }

  va_end(argList);
  output.append("\r\n");

  // file name, line function
  char buffer[1024] = {0};
  snprintf(buffer, 1024, "\t%s(%d) : %s\r\n", szFile, lLine, szFunction);

  output.append(buffer);

#if 0 // defined(OS_WIN)
  OutputDebugString(output.c_str());
#else
  printf(output.c_str());
#endif
}

void CDECL Log(ui::LOG_LEVEL lLevel, const char *szFile,
                 const char *szFunction, long lLine, const char *szFormat,
                 ...) {
  if (lLevel == ui::LOG_LEVEL_DEBUG) {
// #if !defined(_DEBUG)
    return;
// #endif
  }

  // level
  std::string output;
  output.append(LevelToStringA(lLevel));

  // content
  va_list argList;
  va_start(argList, szFormat);

  const int max_size = 10240;
  for (int size = 64; size < max_size; size <<= 2) {
    // 直接在栈上分配
    char *buffer = nullptr;
    buffer = (char*)/*alloca*/ malloc(size);
    int ret = vsnprintf(buffer, size, szFormat, argList);
    if (ret < 0 || ret >= size) {
      free(buffer);
      continue;
    }

    output.append(buffer);

    free(buffer);
    break;
  }

  va_end(argList);
  output.append("\r\n");

  // file name, line function
  char buffer[1024] = {0};
  snprintf(buffer, 1024, "\t%s(%ld) : %s\r\n", szFile, lLine, szFunction);
  output.append(buffer);

#if 0 // defined(OS_WIN)
  OutputDebugStringA(output.c_str());
#else
  printf(output.c_str());
#endif
}

} // namespace ui