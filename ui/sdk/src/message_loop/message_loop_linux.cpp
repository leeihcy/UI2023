#include "message_loop_linux.h"
#include "glib.h"

namespace ui {
// 将xevent作为一个source接入到g_main_loop当中来。
struct XEventSource {
  GSource source;
  MessageLoopPlatformLinux *loop;

  // prepare在poll所有文件描述符之前调用。
  // 如果事件源中有事件发生过（无需通过查看poll调用的结果来确定是否有事件发生），就返回TRUE。
  // prepare可返回一个timeout，该值为poll调用的超时时间（单位为毫秒）。
  // 如果所有事件源返回的timeout均为-1，则poll的超时时间将为-1，
  // 否则poll的超时时间为所有事件源返回的timeout值（不包含-1）中的最小值。
  //
  // 对于无需poll的事件源（例如：idle事件源），其返回TRUE，表示idle事件已经发生了。
  // 对于需要poll的事件源（例如：文件事件源），其返回FALSE，
  // 因为只有在poll文件之后，才能直到文件事件是否发生。
  static gboolean prepare(GSource *source, gint *timeout) {
    *timeout = -1;
    return false;
  }

  // check在poll所有文件描述符之后调用。如果事件已发生，就返回TRUE。
  static gboolean check(GSource *source) {
    XEventSource *xevent_source = (XEventSource *)source;
    return XPending(xevent_source->loop->m_display);
    // return false;
  }

  // 当事件源的prepare或check函数返回TRUE后，说明事件源中有事件发生，
  // 调用dispatch为事件源分发事件。
  // 当需要删除事件源时，dispatch函数返回G_SOURCE_REMOVE，
  // 当需要保留事件源时，dispatch函数返回G_SOURCE_CONTINUE。
  static gboolean dispatch(GSource *source, GSourceFunc callback,
                           gpointer user_data) {
    XEventSource *xevent_source = (XEventSource *)source;
    xevent_source->loop->processXEvent();
    return true;
  }

  // finalize
  // inalize在事件源被销毁前调用。此时，事件源的回调函数已被清除，事件源自身也从GMainContext中删除了，
  // 事件源将要被销毁。但是，事件源可能还持有一些资源的引用计数，因此，可以在此函数中释放这些引用计数。
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

    static GPollFD display_fd = {
      display.fd(),  // fd
      G_IO_IN | G_IO_HUP | G_IO_ERR,  // events
      0  // real events
    };

    // 监听display事件
    g_source_add_poll(source, &display_fd);

    g_source_attach(source, NULL);
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
  this->loop = g_main_loop_new(NULL, false);
  // this->context = g_main_context_default();
  this->context = g_main_loop_get_context(this->loop);

  m_display.Init();

  this->m_xevent_source = XEventSource::Create(this, m_display);
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
  //   while (!this->quit_flag) {
  //     g_main_context_iteration(this->context, true);
  //     this->m_message_loop->OnIdle();
  //   }
  g_main_loop_run(this->loop);
}
void MessageLoopPlatformLinux::Quit() {
  if (this->loop) {
    g_main_loop_quit(this->loop);
  }
  this->quit_flag = true;
  g_main_context_wakeup(nullptr);
}

static int on_idle(gpointer pointer) {
  PostTaskType *p = (PostTaskType *)pointer;
  p->emit();
  delete p;
  return false;
}
void MessageLoopPlatformLinux::PostTask(PostTaskType &&task) {
  auto *p = new PostTaskType(std::forward<PostTaskType>(task));
  g_idle_add(on_idle, gpointer(p));
}

static int on_timer(gpointer pointer) {
  ScheduleTaskType *p = (ScheduleTaskType *)pointer;
  bool continue_run = p->emit();
  if (!continue_run) {
    delete p;
  }
  return continue_run;
}
int MessageLoopPlatformLinux::ScheduleTask(ScheduleTaskType &&task,
                                            int delay_ms) {
  auto *p = new ScheduleTaskType(std::forward<ScheduleTaskType>(task));
  return g_timeout_add(delay_ms, on_timer, gpointer(p));
}

void MessageLoopPlatformLinux::processXEvent(const XEvent &event) {
  XEventDispatcher *dispatcher = m_display.FindDispatcher(event.xany.window);
  if (!dispatcher) {
    return;
  }
  dispatcher->OnXEvent(event);
}

void MessageLoopPlatformLinux::CreateAnimateTimer(int fps) {

}
void MessageLoopPlatformLinux::DestroyAnimateTimer() {
  
}

} // namespace ui
