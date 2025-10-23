#include "sdk/include/util/path.h"

#include <string>
#if defined(OS_MAC)
#include <mach-o/dyld.h>
#elif defined(OS_WIN)
#include "sdk/src/util/windows.h"
#endif
#include "sdk/include/macro/helper.h"

namespace ui {

class PathImpl {
public:
  PathImpl(const char *path_utf8) : m_path(path_utf8) {}

public:
  std::string m_path;
};

// ---------------------------------------------------------

#if defined(OS_WIN)
const char Path::SEPARATOR = '\\';
#else
const char Path::SEPARATOR = '/';
#endif

Path::Path() {}
Path::~Path() {
  if (m_impl) {
    delete m_impl;
    m_impl = nullptr;
  }
}

Path::Path(const Path &o) : m_impl(new PathImpl(o.m_impl->m_path.c_str())) {}

Path::Path(const char *path_utf8) : m_impl(new PathImpl(path_utf8)) {}

/*static */
Path Path::ExecutePath() {

#if defined(OS_WIN)
  wchar_t szPath[256] = {0};
  ::GetModuleFileNameW(nullptr, szPath, 255);
  return Path(CW2A(szPath, CP_UTF8));

#elif defined(OS_MAC)
  // Executable path can have relative references ("..") depending on
  // how the app was launched.
  uint32_t executable_length = 0;
  _NSGetExecutablePath(NULL, &executable_length);

  char *buffer = new char[executable_length + 1];
  _NSGetExecutablePath(buffer, &executable_length);

  char real_path[PATH_MAX] = {0};
  realpath(buffer, real_path);
  delete[] buffer;

  return Path(real_path);

#elif defined(OS_LINUX)
  char path[PATH_MAX] = {0};
  ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
  if (len != -1) {
    path[len] = '\0';
  }
  return Path(path);
#else
  UIASSERT(false);
  return Path();
#endif
}

static inline bool is_separator(const char *p) {
  return *p == '/' || *p == '\\';
}

Path Path::Dir() {
  size_t length = m_impl->m_path.length();
  if (length == 0) {
    return Path();
  }

  const char *pstart = m_impl->m_path.c_str();
  const char *pend = m_impl->m_path.c_str() + length - 1;

  if (is_separator(pend)) {
    pend--;
  }
  while (pstart < pend) {
    if (is_separator(pend)) {
      break;
    }
    pend--;
  }
  return Path(m_impl->m_path.substr(0, pend - pstart).c_str());
}

Path Path::Join(const char *sub_path) {
  if (!sub_path || !sub_path[0]) {
    return *this;
  }
  std::string path_text = m_impl->m_path;
  if (path_text.empty()) {
    return Path(sub_path);
  }

  size_t length = path_text.length();
  if (!is_separator(sub_path) &&
      !is_separator(path_text.c_str() + length - 1)) {
    path_text.append("/");
  }
  if (is_separator(sub_path) && is_separator(path_text.c_str() + length - 1)) {
    sub_path++;
  }

  path_text.append(sub_path);
  return Path(path_text.c_str());
}

bool Path::IsFullPath() {
  
}

const char *Path::ToString() { return m_impl->m_path.c_str(); }

} // namespace ui
