#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_LINUX_H_
#include "window.h"
#include "linux/display.h"


namespace ui
{

class WindowPlatformLinux : public WindowPlatform
{
public:
    void Initialize() override;
    void Release() override;

    void Create(const Rect& rect) override;
    void Attach(::Window window);
    
    void Show() override;
    void Hide();

private:
    X11Display m_display;
    ::Window m_window = 0;
};

}

#endif