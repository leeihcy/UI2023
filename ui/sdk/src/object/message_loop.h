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

  // bool 返回false表示结束运行定时器
  using TimeoutSlot = slot<bool()>;
  int AddTimeout(int elapse, TimeoutSlot&& task);

private:
  void onIdle();

private:
  // 平台相关函数。
  void _init_platform();
  void _destroy_platform();
  void *m_platform_data;

private:
  ui::signal<void()> m_idle_tasks;
};
} // namespace ui

#endif