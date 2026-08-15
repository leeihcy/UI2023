// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mojo/public/cpp/platform/platform_handle.h"

namespace mojo {


PlatformHandle::PlatformHandle() = default;

PlatformHandle::PlatformHandle(PlatformHandle&& other) {
  *this = std::move(other);
}

#if defined(OS_WIN)
PlatformHandle::PlatformHandle(base::win::ScopedHandle handle)
    : type_(Type::kHandle), handle_(std::move(handle)) {}
#endif

PlatformHandle::~PlatformHandle() = default;

PlatformHandle& PlatformHandle::operator=(PlatformHandle&& other) {
  type_ = other.type_;
  other.type_ = Type::kNone;

#if defined (OS_WIN)
  handle_ = std::move(other.handle_);
#endif

  return *this;
}

void PlatformHandle::reset() {
  type_ = Type::kNone;

#if defined(OS_WIN)
  handle_.Close();
#endif

}

void PlatformHandle::release() {
  type_ = Type::kNone;

#if defined (OS_WIN)
  std::ignore = handle_.Take();
#endif

}



}  // namespace mojo
