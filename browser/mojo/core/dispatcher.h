#ifndef MOJO_CORE_DISPATCHER_H_
#define MOJO_CORE_DISPATCHER_H_

#include "mojo/mojo_export.h"
#include "mojo/public/c/system/data_pipe.h"
#include "mojo/public/c/system/types.h"
#include <assert.h>
#include <memory>
#include "mojo/public/c/system/trap.h"

namespace mojo {
  
class PlatformHandle;

namespace core {
class WatcherDispatcher;

namespace ports {
class PortRef;
class UserMessageEvent;
struct PortName;
}  // namespace ports


// MojoHandle的实现类
class MOJO_EXPORT Dispatcher {
public:
  struct DispatcherInTransit {
    DispatcherInTransit() = default;
    DispatcherInTransit(const DispatcherInTransit& other) = default;
    ~DispatcherInTransit() = default;

    std::shared_ptr<Dispatcher> dispatcher;
    MojoHandle local_handle;
  };

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
  virtual ~Dispatcher();
  Dispatcher(const Dispatcher &) = delete;
  Dispatcher &operator=(const Dispatcher &) = delete;

  virtual Type GetType() const = 0;
  virtual MojoResult Close() = 0;

  ///////////// Watcher API ////////////////////
  virtual MojoResult WatchDispatcher(std::shared_ptr<Dispatcher> dispatcher,
                                     MojoHandleSignals signals,
                                     MojoTriggerCondition condition,
                                     uintptr_t context) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  virtual MojoResult CancelWatch(uintptr_t context) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  virtual MojoResult Arm(uint32_t *num_blocking_events,
                         MojoTrapEvent *blocking_events) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

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
  virtual MojoResult WriteData(const void *elements, uint32_t *num_bytes,
                               const MojoWriteDataOptions &options) {
    assert(false);
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  virtual MojoResult BeginWriteData(void **buffer, uint32_t *buffer_num_bytes,
                                    MojoBeginWriteDataFlags flags) {
    assert(false);
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  virtual MojoResult EndWriteData(uint32_t num_bytes_written) {
    assert(false);
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  virtual void StartSerialize(uint32_t *num_bytes, uint32_t *num_ports,
                              uint32_t *num_platform_handles);
  virtual bool EndSerialize(void *destination, ports::PortName *ports,
                            PlatformHandle *handles);

  virtual MojoResult
  AddWatcherRef(const std::shared_ptr<WatcherDispatcher> &watcher,
                uintptr_t context) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  // Removes a WatcherDispatcher reference from this dispatcher.
  virtual MojoResult RemoveWatcherRef(WatcherDispatcher *watcher,
                                      uintptr_t context) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
};

} // namespace core
} // namespace mojo

#endif