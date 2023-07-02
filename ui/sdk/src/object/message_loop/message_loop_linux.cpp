#include "message_loop_linux.h"
#include "glib.h"

namespace ui {

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

static int on_timer(gpointer pointer) {
  TimeoutSlot *p = (TimeoutSlot *)pointer;
  bool continue_run = p->emit();
  if (!continue_run) {
    delete p;
  }
  return continue_run;
}

void MessageLoopPlatformLinux::Initialize(MessageLoop *p) {
  m_message_loop = p;
  this->loop = g_main_loop_new(NULL, FALSE);
  // this->context = g_main_context_default();
  this->context = g_main_loop_get_context(this->loop);

  m_display.Init();
}

void MessageLoopPlatformLinux::Release() {
  m_display.Destroy();

  if (this->context) {
    g_main_context_unref(this->context);
    this->context = nullptr;
  }
  if (this->loop) {
    g_main_loop_unref(this->loop);
    this->loop = nullptr;
  }
}

void MessageLoopPlatformLinux::Run() {
  XEvent event;
  while (!this->quit_flag) {

    // while (XPending(m_display)) {
      XNextEvent(m_display, &event);
      this->processXEvent(event);
      if (this->quit_flag) {
        return;
      }
    // }

    //g_main_context_iteration(this->context, true);
    // this->m_message_loop->OnIdle();
  }
}
void MessageLoopPlatformLinux::Quit() {
  //  if (this->loop) {
  //     g_main_loop_quit(this->loop);
  // }
  this->quit_flag = true;
  g_main_context_wakeup(nullptr);
}

int MessageLoopPlatformLinux::AddTimeout(int elapse, TimeoutSlot &&task) {
  auto *p = new TimeoutSlot(std::forward<TimeoutSlot>(task));

  return g_timeout_add(elapse, on_timer, gpointer(p));
}

void MessageLoopPlatformLinux::processXEvent(const XEvent &event) {
    printf("processXEvent\n");

  XEventDispatcher *dispatcher = m_display.FindDispatcher(event.xany.window);
  if (!dispatcher) {
    return;
  }
  dispatcher->OnXEvent(event);
}

} // namespace ui
