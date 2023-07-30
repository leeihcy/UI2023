#include "inc.h"
#include "include/util/log.h"
#include <string>

std::wstring LevelToString(const ui::LOG_LEVEL &l) {
  static std::wstring strRet;

  switch (l) {
  case ui::LOG_LEVEL_DEBUG:
    return std::wstring(L" [DEBUG]   ");

  case ui::LOG_LEVEL_INFO:
    return std::wstring(L" [INFO]    ");

  case ui::LOG_LEVEL_WARN:
    return std::wstring(L" [WARN]    ");

  case ui::LOG_LEVEL_ERROR:
    return std::wstring(L" [ERROR]   ");

  case ui::LOG_LEVEL_FATAL:
    return std::wstring(L" [FATAL]   ");

  default:
    return std::wstring(L" [UNKNOWN] ");
  }
}

void __cdecl UILog(ui::LOG_LEVEL lLevel, const char *szFile,
                    const char *szFunction, long lLine, const wchar_t *szFormat,
                    ...) {
  // level
  std::wstring output;
  output.append(LevelToString(lLevel));

  // content
  va_list argList;
  va_start(argList, szFormat);

  const int max_size = 10240;
  for (int size = 64; size < max_size; size<<=2) {
    // 直接在栈上分配
    wchar_t *buffer = nullptr;
    buffer = (wchar_t *)/*alloca*/malloc(size*sizeof(wchar_t));
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
	wchar_t buffer[1024] = { 0 };
	swprintf(buffer, 1024, L"\t%s(%d) : %s\r\n", szFile, lLine, szFunction);

	output.append(buffer);

#if defined(OS_WIN)
  OutputDebugString(output.c_str());
#else
  wprintf(output.c_str());
#endif
}
