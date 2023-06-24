#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_H_

#include "../base/signalslot/signal.h"

namespace ui {
class MessageLoop {
public:
  MessageLoop();
  ~MessageLoop();

  void Run();
  void Quit();
  void AddIdleTask(slot<void()>&& task);

private:
  void _init_platform();
  void _destroy_platform();
  void *m_platform_data;

private:
  ui::signal<void()> m_idle_tasks;
};
} // namespace ui

#endif