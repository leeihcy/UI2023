#include "stdafx.h"
#include "Inc/Util/log.h"

namespace UI
{

String LevelToString(const LOG_LEVEL& l)
{
	static String strRet;

	switch (l)
	{
	case LOG_LEVEL_DEBUG:
		return String(_T(" [DEBUG]   "));

	case LOG_LEVEL_INFO:
		return String(_T(" [INFO]    "));

	case LOG_LEVEL_WARN:
		return String(_T(" [WARN]    "));

	case LOG_LEVEL_ERROR:
		return String(_T(" [ERROR]   "));

	case LOG_LEVEL_FATAL:
		return String(_T(" [FATAL]   "));

	default:
		return String(_T(" [UNKNOWN] "));
	}
}

void  _cdecl UILog(LOG_LEVEL lLevel, LPCTSTR szFile, LPCTSTR szFunction, long lLine, LPCTSTR szFormat, ...)
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

void  UIAPI _cdecl UILogA(
	LOG_LEVEL lLevel,
	LPCSTR szFile,
	LPCSTR szFunction,
	long lLine,
	LPCSTR szFormat,
	...)
{
	// TODO:
	
}

}