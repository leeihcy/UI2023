#include "sdk/include/util/path.h"

#include <string>
#if defined(OS_MAC)
#include <mach-o/dyld.h>
#endif

namespace ui
{

class PathImpl {
public:
  PathImpl(const char* path_utf8) : m_path(path_utf8) {
  }
public:
  std::string m_path;
}; 

// ---------------------------------------------------------

Path::Path() {

}
Path::~Path() {

}

Path::Path(const Path& o) : m_impl(new PathImpl(o.m_impl->m_path.c_str())) {

}

Path::Path(const char* path_utf8) : m_impl(new PathImpl(path_utf8)) {
}

/*static */
Path Path::ExecutePath() {

#if defined(OS_WIN)
  wchar_t szPath[256] = {0}; 
  ::GetModuleFileNameW(nullptr, szPath, 255);
  return Path(CW2A(szPath, CF_UTF8));

#elif defined(OS_MAC)
  // Executable path can have relative references ("..") depending on
  // how the app was launched.
  uint32_t executable_length = 0;
  _NSGetExecutablePath(NULL, &executable_length);

  char* buffer = new char[executable_length+1];
  _NSGetExecutablePath(buffer, &executable_length);

  char real_path[PATH_MAX] = {0};
  realpath(buffer, real_path);
  delete[] buffer;

  return Path(real_path);

// #elif defined(OS_LINUX)

#else
  UIASSERT(false);
  return Path();
#endif
}

Path Path::Dir() {

#if defined(OS_MAC)
  int length = m_impl->m_path.length();
  if (length > 0 && m_impl->m_path[length-1] == '/') {
    length--;
  }
  size_t pos = m_impl->m_path.find_last_of("/", length);
  if (pos >= 0) {
    return Path(m_impl->m_path.substr(0, pos).c_str());
  } else {
    return Path("/");
  }
#else
  UIASSERT(false);
  return Path();
#endif
}

Path Path::Join(const char* sub_path) {
  if (!sub_path || !sub_path[0]) {
    return *this;
  }

#if defined(OS_MAC)
 // TODO:
  std::string path_text = m_impl->m_path;
  int length = path_text.length();
  if (sub_path[0] != '/') {
    path_text.append("/");
  }
  path_text.append(sub_path);
  return Path(path_text.c_str());
#else
  UIASSERT(false);
  return Path();
#endif
}

const char* Path::ToString() {
  return m_impl->m_path.c_str();
}

} // namespace ui
