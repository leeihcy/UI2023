#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_H_

#include "include/common/signalslot/signal.h"

namespace ui {

using ScheduleTaskType = slot<bool()>;
using PostTaskType = slot<void()>;

class MessageLoop;

struct MessageLoopPlatform {
  virtual ~MessageLoopPlatform(){};
  virtual void Initialize(MessageLoop *) = 0;
  virtual void Release() = 0;
  virtual void Run() = 0;
  virtual void Quit() = 0;
  virtual void PostTask(PostTaskType &&task) = 0;
  virtual int ScheduleTask(ScheduleTaskType &&task, int delay_ms) = 0;
};

class MessageLoop {
public:
  MessageLoop();
  ~MessageLoop();

  void Run();
  void Quit();

  void PostTask(PostTaskType &&task);
  int ScheduleTask(ScheduleTaskType &&task, int delay_ms);

private:
  MessageLoopPlatform *m_platform = nullptr;
};
} // namespace ui

#endif