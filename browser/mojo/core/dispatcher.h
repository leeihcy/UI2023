#ifndef MOJO_CORE_DISPATCHER_H_
#define MOJO_CORE_DISPATCHER_H_

#include <assert.h>
#include "mojo/mojo_export.h"
#include "mojo/public/c/system/data_pipe.h"
#include "mojo/public/c/system/types.h"


namespace mojo {
namespace core {

// MojoHandle的实现类
class MOJO_EXPORT Dispatcher {
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

  Dispatcher();
  ~Dispatcher();
  Dispatcher(const Dispatcher &) = delete;
  Dispatcher &operator=(const Dispatcher &) = delete;

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

  ///////////// Data pipe consumer API /////////////

  // Supports the the |MojoReadData()| API if implemented by this Dispatcher.
  // Arguments correspond to the ones given to the original API call. See
  // |MojoReadData()| documentation.
  virtual MojoResult ReadData(const MojoReadDataOptions &options,
                              void *elements, uint32_t *num_bytes) {
    assert(false);
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  virtual MojoResult BeginReadData(const void **buffer,
                                   uint32_t *buffer_num_bytes) {
    assert(false);
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  virtual MojoResult EndReadData(uint32_t num_bytes_read) {
    assert(false);
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

};

} // namespace core
} // namespace mojo

#endif