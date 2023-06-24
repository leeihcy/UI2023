#include "message_loop.h"

#include "glib.h"

namespace ui
{

// void  MsgHandleLoop(bool* pbQuitLoopRef)
// {
//     XEvent event;
//     do {        
//         XNextEvent(m_display.GetDisplay(), &event);
//         LinuxWindow* win = LinuxWindow::FromWindow(event.xany.window);
//         if (!win) {
//             printf("Error: no window found\n");
//             continue;
//         }
//         win->OnEvent(event.type, event);

//     } while(m_continue_loop);
// }

namespace {
    struct LinuxData {
        GMainLoop* loop = nullptr;
    };
}

void MessageLoop::_init_platform() 
{
    LinuxData* data = new LinuxData();
    m_platform_data = (void*)data;

    data->loop = g_main_loop_new(NULL, FALSE);
}

void MessageLoop::_destroy_platform()
{
    LinuxData* data = (LinuxData*)m_platform_data;
    if (data->loop) {
        g_main_loop_unref(data->loop);
        data->loop = nullptr;
    }
     
    delete (LinuxData*)m_platform_data;
    m_platform_data = nullptr;
}

void MessageLoop::Run()
{
    LinuxData* data = (LinuxData*)m_platform_data;

    g_main_loop_run(data->loop);
}
void MessageLoop::Quit()
{
    LinuxData* data = (LinuxData*)m_platform_data;

     if (data->loop) {
        g_main_loop_quit(data->loop);
    }
}

}
