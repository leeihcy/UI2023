#include "message_loop.h"
#include "src/application/uiapplication.h"
#if defined(OS_LINUX)
#include "message_loop_linux.h"
#elif defined(OS_MAC)
#include "message_loop_mac.h"
#elif defined(OS_WIN)
#include "message_loop_win.h"
#endif

namespace ui {

MessageLoop::MessageLoop(Application& app) : m_app(app) {

#if defined(OS_LINUX)
  m_platform = new MessageLoopPlatformLinux();
#elif defined(OS_MAC)
  m_platform = new MessageLoopPlatformMac();
#else
  m_platform = new MessageLoopPlatformWin();
#endif
  m_platform->Initialize(this);
}

MessageLoop::~MessageLoop() {
  if (m_platform) {
    m_platform->Release();
    delete m_platform;
    m_platform = nullptr;
  }
}

void MessageLoop::PostTask(PostTaskType &&task) {
  m_platform->PostTask(std::forward<PostTaskType>(task));
}
int MessageLoop::ScheduleTask(ScheduleTaskType &&task, int delay_ms) {

  return m_platform->ScheduleTask(std::forward<ScheduleTaskType>(task), delay_ms);
}

void MessageLoop::CreateAnimateTimer() {
  assert(m_platform);
  m_platform->CreateAnimateTimer();
}
void MessageLoop::DestroyAnimateTimer() {
  assert(m_platform);
  m_platform->DestroyAnimateTimer();
}
void MessageLoop::OnAnimateTimer() {
  m_app.OnAnimateTimer();
}

void MessageLoop::Run() { m_platform->Run(); }

void MessageLoop::Quit() { m_platform->Quit(); }

} // namespace ui
