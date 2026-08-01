#ifndef MOJO_CORE_DISPATCHER_H_
#define MOJO_CORE_DISPATCHER_H_

#include "mojo/public/c/system/types.h"

namespace mojo {
namespace core {

// MojoHandle的实现类
class Dispatcher {
public:
  enum class Type {
    UNKNOWN = 0,
    MESSAGE_PIPE,
    DATA_PIPE_PRODUCER,
    DATA_PIPE_CONSUMER,
    SHARED_BUFFER,
    WATCHER,
    INVITATION,

    // "Private" types (not exposed via the public interface):
    PLATFORM_HANDLE = -1,
  };

  virtual Type GetType() const = 0;
  virtual MojoResult Close() = 0;

  ///////////// Watcher API ////////////////////
  // virtual MojoResult WatchDispatcher(scoped_refptr<Dispatcher> dispatcher,
  //                                    MojoHandleSignals signals,
  //                                    MojoTriggerCondition condition,
  //                                    uintptr_t context);
  // virtual MojoResult CancelWatch(uintptr_t context);
  // virtual MojoResult Arm(uint32_t* num_blocking_events,
  //                        MojoTrapEvent* blocking_events);

  ///////////// Message pipe API /////////////
  // virtual MojoResult WriteMessage(
  //     std::unique_ptr<ports::UserMessageEvent> message);
  // virtual MojoResult ReadMessage(
  //     std::unique_ptr<ports::UserMessageEvent>* message);


};

}
}

#endif