#ifndef _INCLUDED_UI_LOG_H_
#define _INCLUDED_UI_LOG_H_

namespace UI
{

enum LOG_LEVEL
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO = 1,
	LOG_LEVEL_WARN = 2,
	LOG_LEVEL_ERROR = 3,
	LOG_LEVEL_FATAL = 4
};

void  UIAPI _cdecl UILog(
		LOG_LEVEL lLevel,
		LPCTSTR szFile,
		LPCTSTR szFunction, 
		long lLine, 
		LPCTSTR szFormat,
		...);

void  UIAPI _cdecl UILogA(
		LOG_LEVEL lLevel,
		LPCSTR szFile,
		LPCSTR szFunction, 
		long lLine, 
		LPCSTR szFormat,
		...);

#ifdef _UNICODE

	#define UI_LOG_LEVEL(level, format, ...) \
			UI::UILog(level, __FILEW__, __FUNCTIONW__, __LINE__, format, __VA_ARGS__)
	#define UI_LOG_LEVELA(level, format, ...) \
			UI::UILogA(level, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)
#else

	#define UI_LOG_LEVEL(level, format, ...) \
			UI::UILog(level, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)

#endif

#define UI_LOG_DEBUG(format, ... )   UI_LOG_LEVEL(UI::LOG_LEVEL_DEBUG, format, __VA_ARGS__)
#define UI_LOG_INFO(format, ... )    UI_LOG_LEVEL(UI::LOG_LEVEL_INFO,  format, __VA_ARGS__)
#define UI_LOG_WARN(format, ... )    UI_LOG_LEVEL(UI::LOG_LEVEL_WARN,  format, __VA_ARGS__)
#define UI_LOG_ERROR(format, ... )   UI_LOG_LEVEL(UI::LOG_LEVEL_ERROR, format, __VA_ARGS__)
#define UI_LOG_FATAL(format, ... )   UI_LOG_LEVEL(UI::LOG_LEVEL_FATAL, format, __VA_ARGS__)

#define UI_LOG_TEST  UI_LOG_DEBUG

#define UI_LOG_DEBUGA(format, ... )   UI_LOG_LEVELA(UI::LOG_LEVEL_DEBUG, format, __VA_ARGS__)
#define UI_LOG_INFOA(format, ... )    UI_LOG_LEVELA(UI::LOG_LEVEL_INFO,  format, __VA_ARGS__)
#define UI_LOG_WARNA(format, ... )    UI_LOG_LEVELA(UI::LOG_LEVEL_WARN,  format, __VA_ARGS__)
#define UI_LOG_ERRORA(format, ... )   UI_LOG_LEVELA(UI::LOG_LEVEL_ERROR, format, __VA_ARGS__)
#define UI_LOG_FATALA(format, ... )   UI_LOG_LEVELA(UI::LOG_LEVEL_FATAL, format, __VA_ARGS__)

}

#endif