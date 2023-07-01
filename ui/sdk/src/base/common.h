#ifndef _UI_SDK_SRC_BASE_COMMON_H_
#define _UI_SDK_SRC_BASE_COMMON_H_

namespace ui
{

using byte = unsigned char;
using uint = unsigned int;
using ulong = unsigned long;

struct Point {
    int x;
    int y;
};
struct Size
{
    int width;
    int height;
};
struct Rect
{
    int x;
    int y;
    int width;
    int height;
};
struct RectLTRB
{
    int left;
    int top;
    int right;
    int bottom;
};

}
#endif