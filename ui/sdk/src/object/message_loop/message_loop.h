#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_H_

#include "../../base/signalslot/signal.h"

namespace ui {

using TimeoutSlot = slot<bool()>;
class MessageLoop;

struct MessageLoopPlatform {
  virtual ~MessageLoopPlatform(){};
  virtual void Initialize(MessageLoop*) = 0;
  virtual void Release() = 0;
  virtual void Run() = 0;
  virtual void Quit() = 0;
  virtual int AddTimeout(int elapse, TimeoutSlot &&task) = 0;
};

class MessageLoop {
public:
  MessageLoop();
  ~MessageLoop();

  void Run();
  void Quit();

  void AddIdleTask(slot<void()> &&task);

  // bool 返回false表示结束运行定时器
  int AddTimeout(int elapse, TimeoutSlot &&task);

  void OnIdle();

private:
  // 平台相关函数。
  MessageLoopPlatform *m_platform = nullptr;
  ui::signal<void()> m_idle_tasks;
};
} // namespace ui

#endif