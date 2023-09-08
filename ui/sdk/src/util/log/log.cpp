#include "include/util/log.h"
#include "include/inc.h"
#include <string>
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

void __cdecl Log(ui::LOG_LEVEL lLevel, const char *szFile,
                 const char *szFunction, long lLine, const wchar_t *szFormat,
                 ...) {
  // level
  std::wstring output;
  output.append(LevelToString(lLevel));

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
      continue;
    }

    output.append(buffer);
    break;
  }

  va_end(argList);
  output.append(L"\r\n");

  // file name, line function
  wchar_t buffer[1024] = {0};
  swprintf(buffer, 1024, L"\t%s(%d) : %s\r\n", szFile, lLine, szFunction);

  output.append(buffer);

#if 0 // defined(OS_WIN)
  OutputDebugString(output.c_str());
#else
  wprintf(output.c_str());
#endif
}

void __cdecl Log(ui::LOG_LEVEL lLevel, const char *szFile,
                 const char *szFunction, long lLine, const char *szFormat,
                 ...) {
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
      continue;
    }

    output.append(buffer);
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
  printf("%s", output.c_str());
#endif
}

} // namespace ui