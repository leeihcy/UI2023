#ifndef _UI_SDK_HTML_UTIL_UTIL_H
#define _UI_SDK_HTML_UTIL_UTIL_H

#include <string>

namespace html {

bool Utf8ToUnicode(const char* utf8, size_t utf8_length, std::u16string& out_unicode);

}

#endif