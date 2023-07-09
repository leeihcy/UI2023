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

void MessageLoop::AddIdleTask(slot<void()> &&s) {
  m_idle_tasks.connect(std::forward<slot<void()>>(s));
  m_platform->OnAddIdleTask();
}

int MessageLoop::AddTimeout(int elapse, TimeoutSlot &&task) {
    return m_platform->AddTimeout(elapse, std::forward<TimeoutSlot>(task));
}

void MessageLoop::Run() { m_platform->Run(); }

void MessageLoop::Quit() { m_platform->Quit(); }

void MessageLoop::OnIdle() {
  if (m_idle_tasks.empty()) {
    return;
  }
  m_idle_tasks.emit();
  m_idle_tasks.clear();
}

} // namespace ui
