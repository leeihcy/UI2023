// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_PUBLIC_CPP_PLATFORM_PLATFORM_HANDLE_H_
#define MOJO_PUBLIC_CPP_PLATFORM_PLATFORM_HANDLE_H_

#include <utility>
#include "base/win/scoped_handle.h"

namespace mojo {

// A PlatformHandle is a generic wrapper around a platform-specific system
// handle type, e.g. a POSIX file descriptor, Windows HANDLE, or macOS Mach
// port. This can wrap any of various such types depending on the host platform
// for which it's compiled.
//
// This is useful primarily for two reasons:
//
// - Interacting with the Mojo invitation API, which use OS primitives to
//   bootstrap Mojo IPC connections.
// - Interacting with Mojo platform handle wrapping and unwrapping API, which
//   allows handles to OS primitives to be transmitted over Mojo IPC with a
//   stable wire representation via Mojo handles.
//
// NOTE: This assumes ownership if the handle it represents.
class PlatformHandle {
 public:
  enum class Type {
    kNone,
#if defined (OS_WIN)
    kHandle,
// #elif BUILDFLAG(IS_APPLE)
//     kMachSend,
//     kMachReceive,
// #endif
// #if BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
//     kFd,
#endif
  };

  PlatformHandle();
  PlatformHandle(PlatformHandle&& other);

#if defined(OS_WIN)
  explicit PlatformHandle(base::win::ScopedHandle handle);
// #elif BUILDFLAG(IS_FUCHSIA)
//   explicit PlatformHandle(zx::handle handle);
// #elif BUILDFLAG(IS_APPLE)
//   explicit PlatformHandle(base::apple::ScopedMachSendRight mach_port);
//   explicit PlatformHandle(base::apple::ScopedMachReceiveRight mach_port);
#endif

// #if BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
//   explicit PlatformHandle(base::ScopedFD fd);
// #endif

  PlatformHandle(const PlatformHandle&) = delete;
  PlatformHandle& operator=(const PlatformHandle&) = delete;

  ~PlatformHandle();

  PlatformHandle& operator=(PlatformHandle&& other);

  // // Takes ownership of |handle|'s underlying platform handle and fills in
  // // |mojo_handle| with a representation of it. The caller assumes ownership of
  // // the platform handle.
  // static void ToMojoPlatformHandle(PlatformHandle handle,
  //                                  MojoPlatformHandle* mojo_handle);

  // // Closes the underlying platform handle.
  // // Assumes ownership of the platform handle described by |handle|, and returns
  // // it as a new PlatformHandle.
  // static PlatformHandle FromMojoPlatformHandle(
  //     const MojoPlatformHandle* handle);

  Type type() const { return type_; }

  void reset();

  // Relinquishes ownership of the underlying handle, regardless of type, and
  // discards its value. To release and obtain the underlying handle value, use
  // one of the specific |Release*()| methods below.
  void release();

  // Duplicates the underlying platform handle, returning a new PlatformHandle
  // which owns it.
  PlatformHandle Clone() const;

#if defined(OS_WIN)
  bool is_valid() const { return is_valid_handle(); }
  bool is_valid_handle() const { return handle_.is_valid(); }
  bool is_handle() const { return type_ == Type::kHandle; }
  // bool is_pseudo_handle() const {
  //   return base::win::IsPseudoHandle(handle_.get());
  // }
  const base::win::ScopedHandle& GetHandle() const { return handle_; }
  base::win::ScopedHandle TakeHandle() {
    type_ = Type::kNone;
    return std::move(handle_);
  }
  [[nodiscard]] HANDLE ReleaseHandle() {
    type_ = Type::kNone;
    return handle_.release();
  } 
#endif

 private:
  Type type_ = Type::kNone;

#if defined (OS_WIN)
  base::win::ScopedHandle handle_;
#endif

};

}  // namespace mojo

#endif  // MOJO_PUBLIC_CPP_PLATFORM_PLATFORM_HANDLE_H_
