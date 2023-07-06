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

// 将xevent作为一个source接入到g_main_loop当中来。
struct XEventSource {
  GSource source;
  MessageLoopPlatformLinux *loop;

  // 计算下一次的超时时间。
  static gboolean prepare(GSource *source, gint *timeout) { return false; }

  // 检查定时器时间是否到了，如果到了就返回true
  static gboolean check(GSource *source) { return true; }

  // 调用用户注册的回调函数
  static gboolean dispatch(GSource *source, GSourceFunc callback,
                           gpointer user_data) {
    XEventSource *xevent_source = (XEventSource *)source;
    xevent_source->loop->processXEvent();
    return true;
  }

  // finalize
  // event source被移除

  static GSource *Create(MessageLoopPlatformLinux *data, X11Display &display) {
    // 不能用临时变量，g_source内部没有做拷贝。
    static GSourceFuncs source_funcs = {
        prepare, check, dispatch, NULL, /* finalize */
        NULL,                           /* closure_callback */
        NULL                            /* closure_marshal */
    };
    GSource *source = g_source_new(&source_funcs, sizeof(XEventSource));
    ((XEventSource *)source)->loop = data;

    GPollFD display_fd = {display.fd(), G_IO_IN | G_IO_HUP | G_IO_ERR, 0};
    g_source_add_poll(source, &display_fd);
    return source;
  }
  static void Destroy(GSource *source) {
    // GPollFD display_fd = {m_display.fd(), G_IO_IN | G_IO_HUP | G_IO_ERR, 0};
    // g_source_remove_poll(m_xevent_source, &display_fd);
    g_source_destroy(source);
  }
};

void MessageLoopPlatformLinux::Initialize(MessageLoop *p) {
  m_message_loop = p;
  this->loop = g_main_loop_new(NULL, FALSE);
  // this->context = g_main_context_default();
  this->context = g_main_loop_get_context(this->loop);

  m_display.Init();

  this->m_xevent_source = XEventSource::Create(this, m_display);
  g_source_attach(m_xevent_source, this->context);
  g_source_ref(m_xevent_source);
}

void MessageLoopPlatformLinux::Release() {
  if (this->m_xevent_source) {
    XEventSource::Destroy(this->m_xevent_source);
    this->m_xevent_source = nullptr;
  }
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

void MessageLoopPlatformLinux::processXEvent() {
  XEvent event;
  while (XPending(m_display)) {
    XNextEvent(m_display, &event);
    this->processXEvent(event);
  }
}

void MessageLoopPlatformLinux::Run() {
  while (!this->quit_flag) {
    g_main_context_iteration(this->context, true);
    this->m_message_loop->OnIdle();
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
  XEventDispatcher *dispatcher = m_display.FindDispatcher(event.xany.window);
  if (!dispatcher) {
    return;
  }
  dispatcher->OnXEvent(event);
}

} // namespace ui
