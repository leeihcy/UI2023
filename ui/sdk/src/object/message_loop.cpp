#include "message_loop.h"

namespace ui {

MessageLoop::MessageLoop() { _init_platform(); }
MessageLoop::~MessageLoop() { _destroy_platform(); }

void MessageLoop::AddIdleTask(slot<void()>&& s)
{
   m_idle_tasks.connect(std::forward<slot<void()> >(s));
}

} // namespace ui
