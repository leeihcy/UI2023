// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_CORE_WATCH_H_
#define MOJO_CORE_WATCH_H_

#include <memory>
#include "mojo/core/handle_signals_state.h"
#include "mojo/public/c/system/trap.h"

namespace mojo {
namespace core {

class Dispatcher;
class WatcherDispatcher;

class Watch : public std::enable_shared_from_this<Watch>  {
 public:
  // Constructs a Watch which represents a watch within |watcher| associated
  // with |context|, watching |dispatcher| for |signals|.
  Watch(const std::shared_ptr<WatcherDispatcher>& watcher,
        const std::shared_ptr<Dispatcher>& dispatcher,
        uintptr_t context,
        MojoHandleSignals signals,
        MojoTriggerCondition condition);

  Watch(const Watch&) = delete;
  Watch& operator=(const Watch&) = delete;
  ~Watch();

  bool NotifyState(const HandleSignalsState& state,
                   bool allowed_to_call_callback);

  void Cancel();

  void InvokeCallback(MojoResult result,
                      const HandleSignalsState& state,
                      MojoTrapEventFlags flags);

  const std::shared_ptr<Dispatcher>& dispatcher() const { return dispatcher_; }
  uintptr_t context() const { return context_; }

  MojoResult last_known_result() const {
    AssertWatcherLockAcquired();
    return last_known_result_;
  }

  MojoHandleSignalsState last_known_signals_state() const {
    AssertWatcherLockAcquired();
    return last_known_signals_state_;
  }

  bool ready() const {
    AssertWatcherLockAcquired();
    return last_known_result_ == MOJO_RESULT_OK ||
           last_known_result_ == MOJO_RESULT_FAILED_PRECONDITION;
  }

 private:
  void AssertWatcherLockAcquired() const {}
  const std::shared_ptr<WatcherDispatcher> watcher_;
  const std::shared_ptr<Dispatcher> dispatcher_;
  const uintptr_t context_;
  const MojoHandleSignals signals_;
  const MojoTriggerCondition condition_;

  MojoResult last_known_result_ = MOJO_RESULT_UNKNOWN;
  MojoHandleSignalsState last_known_signals_state_ = {0, 0};
  // base::Lock notification_lock_;

  // Guarded by |notification_lock_|.
  bool is_cancelled_ = false;
};

}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_WATCH_H_
