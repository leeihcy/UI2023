#ifndef _UI_SDK_INCLUDE_UTIL_PATH_H_
#define _UI_SDK_INCLUDE_UTIL_PATH_H_
#include <memory>

#include "sdk/include/interface.h"

namespace ui
{
class PathImpl;

class UIAPI Path {
public:
  Path();
  Path(const Path& o);
  Path(const char* path_utf8);
  ~Path();

  static Path ExecutePath();
  Path Dir();
  Path Join(const char* sub_path);

  const char* ToString();

public:
  static const char SEPARATOR;
private:
  PathImpl* m_impl = nullptr;
};

}

#endif