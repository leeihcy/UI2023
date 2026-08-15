#ifndef MOJO_CORE_PLATFORM_HANDLE_IN_TRANSIT_H_
#define MOJO_CORE_PLATFORM_HANDLE_IN_TRANSIT_H_

#include "build/build_config.h"
#include "mojo/public/cpp/platform/platform_handle.h"

#if defined (OS_WIN)
#include <windows.h>
#endif

namespace mojo {
namespace core {

// Owns a PlatformHandle which may actually belong to another process. On
// Windows and (sometimes) Mac, handles in a message object may take on values
// which only have meaning in the context of a remote process.
//
// This class provides a safe way of scoping the lifetime of such handles so
// that they don't leak when transmission can't be completed.
class PlatformHandleInTransit {
 public:
  PlatformHandleInTransit();
  explicit PlatformHandleInTransit(PlatformHandle handle);
  PlatformHandleInTransit(PlatformHandleInTransit&&);

  PlatformHandleInTransit(const PlatformHandleInTransit&) = delete;
  PlatformHandleInTransit& operator=(const PlatformHandleInTransit&) = delete;

  ~PlatformHandleInTransit();

  PlatformHandleInTransit& operator=(PlatformHandleInTransit&&);

  PlatformHandle TakeHandle();

  const PlatformHandle& handle() const {
    // DCHECK(!owning_process_.IsValid());
    return handle_;
  }

#if defined(OS_WIN)
  HANDLE remote_handle() const { return remote_handle_; }
#endif

 private:
#if defined (OS_WIN)
  HANDLE remote_handle_ = INVALID_HANDLE_VALUE;
#endif

  PlatformHandle handle_;
  // base::Process owning_process_;
};

}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_PLATFORM_HANDLE_IN_TRANSIT_H_
