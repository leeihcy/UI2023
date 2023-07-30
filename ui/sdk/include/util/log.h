#ifndef _INCLUDED_UI_LOG_H_
#define _INCLUDED_UI_LOG_H_
#include "../base/uidefine.h"

namespace ui
{

enum LOG_LEVEL
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO = 1,
	LOG_LEVEL_WARN = 2,
	LOG_LEVEL_ERROR = 3,
	LOG_LEVEL_FATAL = 4
};

}


void  UIAPI __cdecl UILog(
		ui::LOG_LEVEL lLevel,
		const char* szFile,
		const char* szFunction, 
		long lLine, 
		const wchar_t* szFormat,
		...);


#define UI_LOG_LEVEL(level, format, ...) \
			UILog(level, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)


#define UI_LOG_DEBUG(format, ... )   UI_LOG_LEVEL(ui::LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define UI_LOG_INFO(format, ... )    UI_LOG_LEVEL(ui::LOG_LEVEL_INFO,  format, ##__VA_ARGS__)
#define UI_LOG_WARN(format, ... )    UI_LOG_LEVEL(ui::LOG_LEVEL_WARN,  format, ##__VA_ARGS__)
#define UI_LOG_ERROR(format, ... )   UI_LOG_LEVEL(ui::LOG_LEVEL_ERROR, format, ##__VA_ARGS__)
#define UI_LOG_FATAL(format, ... )   UI_LOG_LEVEL(ui::LOG_LEVEL_FATAL, format, ##__VA_ARGS__)

#define UI_LOG_TEST  UI_LOG_DEBUG

// #define UI_LOG_DEBUGA(format, ... )   UI_LOG_LEVELA(ui::LOG_LEVEL_DEBUG, format, __VA_ARGS__)
// #define UI_LOG_INFOA(format, ... )    UI_LOG_LEVELA(ui::LOG_LEVEL_INFO,  format, __VA_ARGS__)
// #define UI_LOG_WARNA(format, ... )    UI_LOG_LEVELA(ui::LOG_LEVEL_WARN,  format, __VA_ARGS__)
// #define UI_LOG_ERRORA(format, ... )   UI_LOG_LEVELA(ui::LOG_LEVEL_ERROR, format, __VA_ARGS__)
// #define UI_LOG_FATALA(format, ... )   UI_LOG_LEVELA(ui::LOG_LEVEL_FATAL, format, __VA_ARGS__)

#endif