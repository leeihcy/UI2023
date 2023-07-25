#include "log.h"
#include "inc.h"
#include <string>

namespace ui
{

std::wstring LevelToString(const LOG_LEVEL& l)
{
	static std::wstring strRet;

	switch (l)
	{
	case LOG_LEVEL_DEBUG:
		return std::wstring(L" [DEBUG]   ");

	case LOG_LEVEL_INFO:
		return std::wstring(L" [INFO]    ");

	case LOG_LEVEL_WARN:
		return std::wstring(L" [WARN]    ");

	case LOG_LEVEL_ERROR:
		return std::wstring(L" [ERROR]   ");

	case LOG_LEVEL_FATAL:
		return std::wstring(L" [FATAL]   ");

	default:
		return std::wstring(L" [UNKNOWN] ");
	}
}
void  __cdecl UILog(LOG_LEVEL lLevel, const wchar_t* szFile, const wchar_t* szFunction, long lLine, const wchar_t* szFormat, ...)
{
#ifdef _DEBUG
	// level
	std::wstring strInfo;
	strInfo.append(LevelToString(lLevel));

	// content
	va_list argList;
	va_start(argList, szFormat);

	int nLength = _vsctprintf(szFormat, argList) + 1;
	wchar* pszFormatStack = (wchar*)_malloca(nLength*sizeof(wchar));
	_vstprintf_s(pszFormatStack, nLength, szFormat, argList);
	strInfo.append(pszFormatStack);

	_freea(pszFormatStack); 
	va_end(argList);
	strInfo.append(TEXT("\r\n"));

#if 0
	// file name, line function
	wchar szLine[16] = { 0 };
	wprintf(szLine, TEXT("(%d) : "), lLine);

	strInfo.append(TEXT("\t\t\t"));
	strInfo.append(szFile);
	strInfo.append(szLine);
	strInfo.append(szFunction);

	strInfo.append(TEXT("\r\n"));
#endif
	// output
	OutputDebugString(strInfo.c_str());
#endif
}

void  UIAPI __cdecl UILogA(
	LOG_LEVEL lLevel,
	const char* szFile,
	const char* szFunction,
	long lLine,
	const char* szFormat,
	...)
{
	// TODO:
	
}

}

void  __cdecl UILog2(LOG_LEVEL lLevel, const char* szFile, const char* szFunction, long lLine, const wchar_t* szFormat, ...)
{
    // TODO:
}