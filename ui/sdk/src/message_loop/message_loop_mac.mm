#include "message_loop_mac.h"
#include "../application/application_mac.h"
#import <AppKit/AppKit.h>
#include <cstdio>
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

// 对于 event loop 的具体实现有两个：
// 1. Foundation 框架中的 NSRunLoop
// 2. Core Foundation 框架中的 CFRunLoop
// 其中 NSRunLoop 是对 CFRunLoop 的简单封装

const CFTimeInterval kCFTimeIntervalMax =
    std::numeric_limits<CFTimeInterval>::max();

namespace ui {

// static
void MessageLoopPlatformMac::onIdleEntry(void *info) {
  MessageLoopPlatformMac *pthis = static_cast<MessageLoopPlatformMac *>(info);
  pthis->onIdle();
}

void MessageLoopPlatformMac::Initialize(MessageLoop *p) {
  ApplicationMac::Init();

  this->m_message_loop = p;

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

  DestroyAnimateTimer();

  CFRunLoopRemoveSource(run_loop, m_idle_source, kCFRunLoopCommonModes);
  CFRelease(m_idle_source);
}

// 只运行CFRunLoopRunInMode，ui不会有响应。得运行NsApp run才行。
//
// 主线程得以NSApplication运行，其它线程以NSRunLoop方式运行
// https://chromium.googlesource.com/chromium/src/base/+/2103662f9a6cb031cd5b01c0fb0ef20caea181c3/message_pump_mac.mm
//
/*
2 com.apple.CoreFoundation 0x9358b37f __CFRunLoopRun + 2079
3 com.apple.CoreFoundation 0x9358a464 CFRunLoopRunSpecific + 452
4 com.apple.CoreFoundation 0x9358a291 CFRunLoopRunInMode + 97
5 com.apple.HIToolbox 0x99512f9c RunCurrentEventLoopInMode + 392
6 com.apple.HIToolbox 0x99512d51 ReceiveNextEventCommon + 354
7 com.apple.HIToolbox 0x99512bd6 BlockUntilNextEventMatchingListInMode + 81
8 com.apple.AppKit 0x949f978d _DPSNextEvent + 847
9 com.apple.AppKit 0x949f8fce -[NSApplication
nextEventMatchingMask:untilDate:inMode:dequeue:] + 156 10 com.apple.AppKit
0x949bb247 -[NSApplication run] + 821 11 com.apple.AppKit 0x949b32d9
NSApplicationMain + 574 _start + 208 start + 40
*/
void MessageLoopPlatformMac::Run() {

  if (m_type == MacMessageLoopType::_NSAppRun) {
    [NSApp run];
  } else if (m_type == MacMessageLoopType::_CFRunLoop) {
    int result = 0;
    do {
      result =
          CFRunLoopRunInMode(kCFRunLoopDefaultMode, kCFTimeIntervalMax, false);
    } while (result != kCFRunLoopRunStopped && result != kCFRunLoopRunFinished);
  }
}
void MessageLoopPlatformMac::Quit() {
  if (m_type == MacMessageLoopType::_NSAppRun) {
    [NSApp stop:nil];

    // stop 只是加一个标记，将在下一个事件处理完之后退出。
    // Send a fake event to wake the loop up.
    [NSApp postEvent:[NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:NULL
                                         subtype:0
                                           data1:0
                                           data2:0]
             atStart:NO];
  } else if (m_type == MacMessageLoopType::_CFRunLoop) {
    auto run_loop = CFRunLoopGetCurrent();
    CFRunLoopStop(run_loop);
  }
}

struct TimerDataMac {
  TimerDataMac(MessageLoopPlatformMac *p, ScheduleTaskType &&task)
      : pthis(p),
        timeout_slot(std::forward<ScheduleTaskType>(task)) //, timer(nil)
  {}
  MessageLoopPlatformMac *pthis;
  ScheduleTaskType timeout_slot;
};

static void onTimerEntry(__CFRunLoopTimer *timer, void *info) {
  TimerDataMac *data = static_cast<TimerDataMac *>(info);

  bool continue_run = data->timeout_slot.emit();
  if (!continue_run) {
    CFRunLoopRemoveTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
    delete data;
  }
}

void MessageLoopPlatformMac::onIdle() {
  if (m_idle_tasks.empty()) {
    return;
  }
  m_idle_tasks.emit();
  m_idle_tasks.clear();
}

void MessageLoopPlatformMac::PostTask(PostTaskType &&task) {
  m_idle_tasks.connect(std::forward<PostTaskType>(task));

  CFRunLoopSourceSignal(m_idle_source);
  CFRunLoopWakeUp(CFRunLoopGetCurrent());
}
int MessageLoopPlatformMac::ScheduleTask(ScheduleTaskType &&task,
                                         int delay_ms) {
  CFRunLoopTimerContext timer_context = CFRunLoopTimerContext();

  timer_context.info =
      new TimerDataMac(this, std::forward<ScheduleTaskType>(task));
  CFRunLoopTimerRef timer = CFRunLoopTimerCreate(NULL, // allocator
                                                 CFAbsoluteTimeGetCurrent(),
                                                 delay_ms / 1000, // interval
                                                 0,               // flags
                                                 0,               // priority
                                                 onTimerEntry, &timer_context);
  CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);

  return 0;
}


static void onAnimateTimer(__CFRunLoopTimer *timer, void *info) {
  MessageLoopPlatformMac* pthis = (MessageLoopPlatformMac*)info;
  // 触发这个回调时，应该可以认为MessageLoopPlatformMac还活着。
  pthis->OnAnimateTimer();
}

void MessageLoopPlatformMac::OnAnimateTimer() {
  m_message_loop->OnAnimateTimer();
}

void MessageLoopPlatformMac::CreateAnimateTimer() {
  if (m_animate_timer != nullptr) {
    return;
  }

  CFRunLoopTimerContext timer_context = CFRunLoopTimerContext();
  timer_context.info = this;

  // mac的timer能支持高精度的定时
  // 60fps
  m_animate_timer = CFRunLoopTimerCreate(NULL, // allocator
                                                 CFAbsoluteTimeGetCurrent(),
                                                 1/60.0,       // interval
                                                 0,               // flags
                                                 0,               // priority
                                                 onAnimateTimer, &timer_context);
  CFRunLoopAddTimer(CFRunLoopGetCurrent(), m_animate_timer, kCFRunLoopCommonModes);
}
void MessageLoopPlatformMac::DestroyAnimateTimer() {
  if (!m_animate_timer) {
    return;
  }
  CFRunLoopRemoveTimer(CFRunLoopGetCurrent(), m_animate_timer, kCFRunLoopCommonModes);
  m_animate_timer = nullptr;
}

} // namespace ui