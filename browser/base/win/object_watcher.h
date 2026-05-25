#ifndef BASE_WIN_OBJECT_WATCHER_H_
#define BASE_WIN_OBJECT_WATCHER_H_

#include "base/win/windows_types.h"

namespace base {
namespace win {

class ObjectWatcher {
public:
  ObjectWatcher();
  ~ObjectWatcher();

  class Delegate {
  public:
    virtual ~Delegate() = default;
    virtual void OnObjectSignaled(HANDLE object) = 0;
  };

  bool StartWatchingOnce(HANDLE object, Delegate *delegate);
  bool StartWatchingMultipleTimes(HANDLE object, Delegate *delegate);

  bool StopWatching();
  void Reset();
  void OnSignal();

private:
  bool StartWatchingInternal(HANDLE object, Delegate *delegate,
                             bool execute_only_once);

private:
  // The object being watched.
  HANDLE object_ = nullptr;

  // The wait handle returned by RegisterWaitForSingleObject.
  HANDLE wait_object_ = nullptr;

  Delegate* delegate_ = nullptr;

  bool run_once_ = true;
};


}
}

#endif