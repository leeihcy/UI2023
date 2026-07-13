#include "base/win/object_watcher.h"

#include <windows.h>

namespace {

const char kMessageWindowClass[] = "ObjectWatcherMessageWindow";

// Custom message sent via HWND_MESSAGE window to dispatch OnSignal()
// back to the original calling thread. 0x0400 is WM_USER.
const unsigned int kMsgObjectSignaled = 0x0400 + 0x100;

LRESULT CALLBACK ObjectWatcherWndProc(HWND hwnd, UINT msg,
                                      WPARAM wparam, LPARAM lparam) {
  if (msg == kMsgObjectSignaled) {
    reinterpret_cast<base::win::ObjectWatcher*>(lparam)->OnSignal();
    return 0;
  }
  return ::DefWindowProc(hwnd, msg, wparam, lparam);
}

HWND GetMessageWindow() {
  static HWND hwnd = []() {
    WNDCLASSA wc = {};
    wc.lpfnWndProc = ObjectWatcherWndProc;
    wc.hInstance = reinterpret_cast<HINSTANCE>(::GetModuleHandle(nullptr));
    wc.lpszClassName = kMessageWindowClass;
    ::RegisterClassA(&wc);

    return ::CreateWindowExA(0, kMessageWindowClass, nullptr, 0,
                            0, 0, 0, 0, HWND_MESSAGE, nullptr,
                            wc.hInstance, nullptr);
  }();
  return hwnd;
}

}  // namespace

namespace base {
namespace win {

ObjectWatcher::ObjectWatcher() = default;

ObjectWatcher::~ObjectWatcher() {
  StopWatching();
}

void ObjectWatcher::Reset() {
  object_ = nullptr;
  wait_object_ = nullptr;
  run_once_ = true;
}

bool ObjectWatcher::StartWatchingOnce(HANDLE object,
                                      Delegate* delegate) {
  return StartWatchingInternal(object, delegate, true);
}

bool ObjectWatcher::StartWatchingMultipleTimes(HANDLE object,
                                               Delegate* delegate) {
  return StartWatchingInternal(object, delegate, false);
}

bool ObjectWatcher::StopWatching() {
  if (!wait_object_) {
    return false;
  }
  {
    UnregisterWaitEx(wait_object_, nullptr);
  }
  Reset();
  return true;
}

static
void CALLBACK DoneWaiting(void* param, BOOLEAN timed_out) {
  ObjectWatcher* that = static_cast<ObjectWatcher*>(param);
  ::PostMessage(GetMessageWindow(), kMsgObjectSignaled, 0,
                reinterpret_cast<LPARAM>(that));
}

bool ObjectWatcher::StartWatchingInternal(HANDLE object,
                                          Delegate* delegate,
                                          bool execute_only_once) {
  delegate_ = delegate;
  object_ = object;

  // Ensure the message-only window is created on this thread so that
  // OnSignal() is dispatched back to this thread's message loop.
  GetMessageWindow();

  run_once_ = execute_only_once;

  // Since our job is to just notice when an object is signaled and report the
  // result back to this sequence, we can just run on a Windows wait thread.
  DWORD wait_flags = WT_EXECUTEINWAITTHREAD;
  if (run_once_) {
    wait_flags |= WT_EXECUTEONLYONCE;
  }

  if (!RegisterWaitForSingleObject(&wait_object_, object, DoneWaiting, this,
                                   INFINITE, wait_flags)) {
    return false;
  }

  return true;
}

void ObjectWatcher::OnSignal() {
    if (!delegate_) {
        return;
    }
    HANDLE object = object_;
    Delegate* delegate = delegate_;
    if (run_once_) {
        StopWatching();
    }
    delegate->OnObjectSignaled(object);
  }
}

}