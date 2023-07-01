#include "message_loop.h"
#if defined(OS_MAC) || defined(OS_LINUX)
#include "message_loop_linux.h"
#endif

namespace ui {

MessageLoop::MessageLoop() { 

#if defined(OS_MAC) || defined(OS_LINUX)
    m_platform = new MessageLoopPlatformLinux();
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

void MessageLoop::AddIdleTask(slot<void()>&& s)
{
   m_idle_tasks.connect(std::forward<slot<void()> >(s));
}

void MessageLoop::Run() {
    m_platform->Run();
}

void MessageLoop::OnIdle()
{
    if (m_idle_tasks.empty()) {
        return;
    }
    m_idle_tasks.emit();
    m_idle_tasks.clear();
}

} // namespace ui
