#pragma once

// enum LOG_LEVEL
// {
// 	LOG_LEVEL_DEBUG = 0,
// 	LOG_LEVEL_INFO = 1,
// 	LOG_LEVEL_WARN = 2,
// 	LOG_LEVEL_ERROR = 3,
// 	LOG_LEVEL_FATAL = 4
// };

// void  __cdecl UILog(LOG_LEVEL lLevel, const wchar_t* szFile, const wchar_t* szFunction, long lLine, const wchar_t* szFormat, ...);
// void  __cdecl UILog2(LOG_LEVEL lLevel, const char* szFile, const char* szFunction, long lLine, const wchar_t* szFormat, ...);

// #ifdef _UNICODE

// 	#define UI_LOG_LEVEL(level, format, ...) \
// 			UILog(level, __FILEW__, __FUNCTIONW__, __LINE__, format, #__VA_ARGS__)

// #else

// 	#define UI_LOG_LEVEL(level, format, ...) \
// 			UILog2(level, __FILE__, __FUNCTION__, __LINE__, format, #__VA_ARGS__)

// #endif

// #define UI_LOG_DEBUG(format, ... )   UI_LOG_LEVEL(LOG_LEVEL_DEBUG, format, #__VA_ARGS__)
// #define UI_LOG_INFO(format, ... )    UI_LOG_LEVEL(LOG_LEVEL_INFO,  format, #__VA_ARGS__)
// #define UI_LOG_WARN(format, ... )    UI_LOG_LEVEL(LOG_LEVEL_WARN,  format, #__VA_ARGS__)
// #define UI_LOG_ERROR(format, ... )   UI_LOG_LEVEL(LOG_LEVEL_ERROR, format, #__VA_ARGS__)
// #define UI_LOG_FATAL(format, ... )   UI_LOG_LEVEL(LOG_LEVEL_FATAL, format, #__VA_ARGS__)

// #define UI_LOG_TEST  UI_LOG_DEBUG
