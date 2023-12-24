#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_H_

#include "include/common/signalslot/signal.h"
#include "include/interface/ianimate.h"
#include "include/interface/iuiapplication.h"
#include <memory>

namespace ui {

using ScheduleTaskType = slot<bool()>;
using PostTaskType = slot<void()>;

class MessageLoop;
class Application;

struct MessageLoopPlatform {
  virtual ~MessageLoopPlatform(){};
  virtual void Initialize(MessageLoop *) = 0;
  virtual void Release() = 0;
  virtual void Run() = 0;
  virtual void Quit() = 0;
  virtual void PostTask(PostTaskType &&task) = 0;
  virtual int ScheduleTask(ScheduleTaskType &&task, int delay_ms) = 0;
  virtual void CreateAnimateTimer() = 0;
  virtual void DestroyAnimateTimer() = 0;
};

class MessageLoop {
public:
  MessageLoop(Application& app);
  ~MessageLoop();

  void Run();
  void Quit();

  void PostTask(PostTaskType &&task);
  int ScheduleTask(ScheduleTaskType &&task, int delay_ms);

  void CreateAnimateTimer();
  void DestroyAnimateTimer();
  void OnAnimateTimer();

  Application& GetApplication() { return m_app; }

private:
  Application& m_app;
  MessageLoopPlatform *m_platform = nullptr;
};
} // namespace ui

#endif