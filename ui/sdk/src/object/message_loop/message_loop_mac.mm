#import "Cocoa/Cocoa.h"

#include "message_loop_mac.h"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

const CFTimeInterval kCFTimeIntervalMax =
    std::numeric_limits<CFTimeInterval>::max();

namespace ui {

// static
void MessageLoopPlatformMac::onIdleEntry(void *info) {
  MessageLoopPlatformMac *pthis = static_cast<MessageLoopPlatformMac *>(info);
  pthis->onIdle();
}

void MessageLoopPlatformMac::Initialize(MessageLoop *p) {
  this->m_message_loop = p;

  //   [NSApplication sharedApplication];
  auto run_loop = CFRunLoopGetCurrent();

  //   CFRunLoopSourceCreate
  CFRunLoopSourceContext source_context = CFRunLoopSourceContext();
  source_context.info = this;
  source_context.perform = onIdleEntry;
  m_idle_source = CFRunLoopSourceCreate(NULL, // allocator
                                        2,    // priority
                                        &source_context);
  CFRunLoopAddSource(run_loop, m_idle_source, kCFRunLoopCommonModes);
}

void MessageLoopPlatformMac::Release() {
  auto run_loop = CFRunLoopGetCurrent();
  CFRunLoopRemoveSource(run_loop, m_idle_source, kCFRunLoopCommonModes);
  CFRelease(m_idle_source);
}

void MessageLoopPlatformMac::Run() {
  int result = 0;
  do {
    result =
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, kCFTimeIntervalMax, false);
  } while (result != kCFRunLoopRunStopped && result != kCFRunLoopRunFinished);
}
void MessageLoopPlatformMac::Quit() {
  auto run_loop = CFRunLoopGetCurrent();
  CFRunLoopStop(run_loop);
}

struct TimerDataMac {
  TimerDataMac(MessageLoopPlatformMac *p, TimeoutSlot &&task)
      : pthis(p), timeout_slot(std::forward<TimeoutSlot>(task)) //, timer(nil)
  {}
  MessageLoopPlatformMac *pthis;
  TimeoutSlot timeout_slot;
};

static void onTimerEntry(__CFRunLoopTimer *timer, void *info) {
  TimerDataMac *data = static_cast<TimerDataMac *>(info);

  bool continue_run = data->timeout_slot.emit();
  if (!continue_run) {
    CFRunLoopRemoveTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
    delete data;
  }
}

void MessageLoopPlatformMac::onIdle() { this->m_message_loop->OnIdle(); }

int MessageLoopPlatformMac::AddTimeout(int elapse, TimeoutSlot &&task) {
  CFRunLoopTimerContext timer_context = CFRunLoopTimerContext();

  timer_context.info = new TimerDataMac(this, std::forward<TimeoutSlot>(task));
  CFRunLoopTimerRef timer = CFRunLoopTimerCreate(NULL, // allocator
                                                 CFAbsoluteTimeGetCurrent(),
                                                 elapse / 1000, // interval
                                                 0,             // flags
                                                 0,             // priority
                                                 onTimerEntry, &timer_context);
  CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);

  return 0;
}

void MessageLoopPlatformMac::OnAddIdleTask() {
  CFRunLoopSourceSignal(m_idle_source);
  CFRunLoopWakeUp(CFRunLoopGetCurrent());
}

} // namespace ui