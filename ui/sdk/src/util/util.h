#ifndef _UI_SDK_SRC_UTIL_UTIL_H_
#define _UI_SDK_SRC_UTIL_UTIL_H_
#include "include/util/color.h"

namespace ui {
struct Size;
struct Rect;
struct C9Region;

namespace util {
Color TranslateHexColor(const char *szColor);
Color TranslateColor(const char *szColor);
Color TranslateRGB(const char *szCol, char szSep = ',');
bool TranslateSIZE(const char *szText, Size *pSize, char szSep = ',');
bool TranslateRECT(const char *szRect, Rect *pRect, char szSep = ',');
bool TranslateImage9Region(const char *str, C9Region *p9Region,
                           char szSep = ',');

// TODO add Path class
bool Path_IsDirectory(const char *path);
bool Path_FileExists(const char *path);

int wtoi(const wchar_t *text);
}


}

#endif