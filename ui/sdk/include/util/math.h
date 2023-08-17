#ifndef _UI_SDK_INCLUDE_UTIL_MATH_H_
#define _UI_SDK_INCLUDE_UTIL_MATH_H_

namespace ui
{

inline int _round(const float &f) {
  if (f < 0.0f)
    return int(f - 0.5f);
  else
    return int(f + 0.5f);
}
inline int _round(const double &f) {
  if (f < 0.0f)
    return int(f - 0.5);
  else
    return int(f + 0.5);
}

}

#endif