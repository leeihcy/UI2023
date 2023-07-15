#include "util/log.h"

namespace ui
{

String LevelToString(const LOG_LEVEL& l)
{
	static String strRet;

	switch (l)
	{
	case LOG_LEVEL_DEBUG:
		return String(L" [DEBUG]   ");

	case LOG_LEVEL_INFO:
		return String(L" [INFO]    ");

	case LOG_LEVEL_WARN:
		return String(L" [WARN]    ");

	case LOG_LEVEL_ERROR:
		return String(L" [ERROR]   ");

	case LOG_LEVEL_FATAL:
		return String(L" [FATAL]   ");

	default:
		return String(L" [UNKNOWN] ");
	}
}
void  __cdecl UILog2(LOG_LEVEL lLevel, const char* szFile, const char* szFunction, long lLine, const wchar_t* szFormat, ...)
{
    // TODO:
}
void  __cdecl UILog(LOG_LEVEL lLevel, const wchar_t* szFile, const wchar_t* szFunction, long lLine, const wchar_t* szFormat, ...)
{
#ifdef _DEBUG
	// level
	String strInfo;
	strInfo.append(LevelToString(lLevel));

	// content
	va_list argList;
	va_start(argList, szFormat);

	int nLength = _vsctprintf(szFormat, argList) + 1;
	TCHAR* pszFormatStack = (TCHAR*)_malloca(nLength*sizeof(TCHAR));
	_vstprintf_s(pszFormatStack, nLength, szFormat, argList);
	strInfo.append(pszFormatStack);

	_freea(pszFormatStack); 
	va_end(argList);
	strInfo.append(TEXT("\r\n"));

#if 0
	// file name, line function
	TCHAR szLine[16] = { 0 };
	_stprintf(szLine, TEXT("(%d) : "), lLine);

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