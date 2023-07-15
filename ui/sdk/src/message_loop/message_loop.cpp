#include "message_loop.h"
#if defined(OS_LINUX) || defined(USE_LINUX_MSGLOOP)
#include "message_loop_linux.h"
#elif defined(OS_MAC)
#include "message_loop_mac.h"
#endif

namespace ui {

MessageLoop::MessageLoop() {

#if defined(OS_LINUX) || defined(USE_LINUX_MSGLOOP)
  m_platform = new MessageLoopPlatformLinux();
#elif defined(OS_MAC)
  m_platform = new MessageLoopPlatformMac();
#else
  assert(false);
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

void MessageLoop::Run() { m_platform->Run(); }

void MessageLoop::Quit() { m_platform->Quit(); }

} // namespace ui
