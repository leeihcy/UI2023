#include "mojo/core/platform_handle_in_transit.h"

#include <utility>

namespace mojo {
namespace core {


PlatformHandleInTransit::PlatformHandleInTransit() = default;

PlatformHandleInTransit::PlatformHandleInTransit(PlatformHandle handle)
    : handle_(std::move(handle)) {}

PlatformHandleInTransit::PlatformHandleInTransit(
    PlatformHandleInTransit&& other) {
  *this = std::move(other);
}

PlatformHandleInTransit::~PlatformHandleInTransit() {
#if BUILDFLAG(IS_WIN)
  // if (!owning_process_.IsValid()) {
  //   DCHECK_EQ(remote_handle_, INVALID_HANDLE_VALUE);
  //   return;
  // }

  // CloseHandleInProcess(remote_handle_, owning_process_);
#endif
}

PlatformHandleInTransit& PlatformHandleInTransit::operator=(
    PlatformHandleInTransit&& other) {
// #if BUILDFLAG(IS_WIN)
//   if (owning_process_.IsValid()) {
//     DCHECK_NE(remote_handle_, INVALID_HANDLE_VALUE);
//     CloseHandleInProcess(remote_handle_, owning_process_);
//   }

//   remote_handle_ = INVALID_HANDLE_VALUE;
//   std::swap(remote_handle_, other.remote_handle_);
// #endif
  handle_ = std::move(other.handle_);
  // owning_process_ = std::move(other.owning_process_);
  return *this;
}

PlatformHandle PlatformHandleInTransit::TakeHandle() {
  // DCHECK(!owning_process_.IsValid());
  return std::move(handle_);
}

}  // namespace core
}  // namespace mojo
