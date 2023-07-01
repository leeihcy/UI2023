#include "window.h"
#include <assert.h>

#if defined(OS_MAC) || defined(OS_LINUX)
#include "window_linux.h"
#endif

namespace ui
{

Window::Window() { 
}
Window::~Window() { 
    if (m_platform) {
        m_platform->Release();
        delete m_platform;
        m_platform = nullptr;
    }
}

void Window::Create(const Rect& rect) {
#if defined(OS_WIN)
#elif defined(OS_MAC) ||  defined(OS_LINUX)
    m_platform = new WindowPlatformLinux();
    m_platform->Initialize();
#else
    assert(false);
#endif
    m_platform->Create(rect);
}

  void Window::Show()
  {
    if (m_platform) {
        m_platform->Show();
    }
  }
}