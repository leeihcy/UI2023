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
    struct MacData {
        bool quit_flag = false;

        // NSRunLoop;
    };
}

void MessageLoop::_init_platform() 
{
    MacData* data = new MacData();
    m_platform_data = (void*)data;
}

void MessageLoop::_destroy_platform()
{
    MacData* data = (MacData*)m_platform_data;
     
    delete (LinuxData*)m_platform_data;
    m_platform_data = nullptr;
}

void MessageLoop::Run()
{
    MacData* data = (MacData*)m_platform_data;

    // g_main_loop_run(data->loop);

    // while (!data->quit_flag) {
    //     g_main_context_iteration(data->context, true);

    //     this->onIdle();
    // }
}
void MessageLoop::Quit()
{
    MacData* data = (MacData*)m_platform_data;
    data->quit_flag = true;
}

}
