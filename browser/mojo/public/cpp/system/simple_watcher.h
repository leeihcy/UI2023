// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_PUBLIC_CPP_SYSTEM_SIMPLE_WATCHER_H_
#define MOJO_PUBLIC_CPP_SYSTEM_SIMPLE_WATCHER_H_

#include <functional>
#include <memory>
#include "mojo/mojo_export.h"
#include "mojo/public/c/system/types.h"
#include "mojo/public/cpp/system/handle_signals_state.h"
#include "mojo/public/cpp/system/handle.h"
#include "mojo/public/c/system/trap.h"

namespace base {
class SequencedTaskRunner;
}

namespace mojo {

class MOJO_EXPORT SimpleWatcher {
 public:
   using ReadyCallback = std::function<void(MojoResult result)>;

   using ReadyCallbackWithState =
       std::function<void(MojoResult result, const HandleSignalsState &state)>;

   enum class ArmingPolicy {
     AUTOMATIC,
     MANUAL,
   };

   SimpleWatcher(ArmingPolicy arming_policy, const char *handler_tag = nullptr);

   SimpleWatcher(const SimpleWatcher &) = delete;
   SimpleWatcher &operator=(const SimpleWatcher &) = delete;

   ~SimpleWatcher();

   bool IsWatching() const;
   MojoResult Watch(Handle handle, MojoHandleSignals signals,
                    MojoTriggerCondition condition,
                    ReadyCallbackWithState callback);

   MojoResult Watch1(Handle handle, MojoHandleSignals signals,
                    ReadyCallback callback) {
     return Watch(handle, signals, MOJO_WATCH_CONDITION_SATISFIED,
                  std::bind(&DiscardReadyState, std::move(callback), std::placeholders::_1, std::placeholders::_2));
  }

  void Cancel();

  MojoResult Arm(MojoResult* ready_result = nullptr,
                 HandleSignalsState* ready_state = nullptr);

  void ArmOrNotify();

  Handle handle() const { return handle_; }
  ReadyCallbackWithState ready_callback() const { return callback_; }

 private:
  class Context;

  static void DiscardReadyState(const ReadyCallback& callback,
                                MojoResult result,
                                const HandleSignalsState& state) {
    callback(result);
  }

  void OnHandleReady(int watch_id,
                     MojoResult result,
                     const HandleSignalsState& state);

  // The policy used to determine how this SimpleWatcher is armed.
  const ArmingPolicy arming_policy_;

  // ScopedTrapHandle trap_handle_;

  // A thread-safe context object corresponding to the currently active watch,
  // if any.
  std::shared_ptr<Context> context_;

  // Fields below must only be accessed on the SimpleWatcher's owning sequence.

  // The handle currently under watch. Not owned.
  Handle handle_;

  // A simple counter to disambiguate notifications from multiple watch contexts
  // in the event that this SimpleWatcher cancels and watches multiple times.
  int watch_id_ = 0;

  // The callback to call when the handle is signaled.
  ReadyCallbackWithState callback_;

  // Tag used to ID memory allocations that originated from notifications in
  // this watcher.
  const char* handler_tag_ = nullptr;
};

}  // namespace mojo

#endif  // MOJO_PUBLIC_CPP_SYSTEM_SIMPLE_WATCHER_H_
