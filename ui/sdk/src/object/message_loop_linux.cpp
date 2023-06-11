#include "message_loop.h"

namespace ui
{

void  MsgHandleLoop(bool* pbQuitLoopRef)
{
    XEvent event;
    do {        
        XNextEvent(m_display.GetDisplay(), &event);
        LinuxWindow* win = LinuxWindow::FromWindow(event.xany.window);
        if (!win) {
            printf("Error: no window found\n");
            continue;
        }
        win->OnEvent(event.type, event);

    } while(m_continue_loop);
}

}
