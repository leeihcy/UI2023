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
        bool quit_flag = false;

        GMainLoop* loop = nullptr;
        GMainContext* context = nullptr;;
    };

    int on_timer(gpointer pointer) 
    {
        MessageLoop::TimeoutSlot* p = (MessageLoop::TimeoutSlot*)pointer;
        bool continue_run = p->emit();
        if (!continue_run) {
            delete p;
        }
        return continue_run;
    }
}

void MessageLoop::_init_platform() 
{
    LinuxData* data = new LinuxData();
    m_platform_data = (void*)data;

    data->loop = g_main_loop_new(NULL, FALSE);
    //data->context = g_main_context_default();
    data->context = g_main_loop_get_context(data->loop);
}

void MessageLoop::_destroy_platform()
{
    LinuxData* data = (LinuxData*)m_platform_data;
    if (data->context) {
        g_main_context_unref(data->context);
        data->context = nullptr;
    }
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

    // g_main_loop_run(data->loop);

    while (!data->quit_flag) {
        g_main_context_iteration(data->context, true);

        this->onIdle();
    }
}
void MessageLoop::Quit()
{
    LinuxData* data = (LinuxData*)m_platform_data;

    //  if (data->loop) {
    //     g_main_loop_quit(data->loop);
    // }
    data->quit_flag = true;
    g_main_context_wakeup(nullptr);
}


int MessageLoop::AddTimeout(int elapse, TimeoutSlot&& task)
{
    auto* p = new TimeoutSlot(
        std::forward<TimeoutSlot>(task));

    return g_timeout_add(elapse, on_timer, gpointer(p));
}

}
