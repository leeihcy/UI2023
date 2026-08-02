// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MEMORY_SHARED_MEMORY_MAPPER_H_
#define BASE_MEMORY_SHARED_MEMORY_MAPPER_H_

#include <stdint.h>

#include <optional>
#include <span>
#include "base/memory/platform_shared_memory_handle.h"

namespace base {

class SharedMemoryMapper {
 public:
  static SharedMemoryMapper* GetDefaultInstance();

  virtual std::optional<std::span<uint8_t>> Map(
      subtle::PlatformSharedMemoryHandle handle,
      bool write_allowed,
      uint64_t offset,
      size_t size) = 0;

  virtual void Unmap(std::span<uint8_t> mapping) = 0;
};

}  // namespace base

#endif  // BASE_MEMORY_SHARED_MEMORY_MAPPER_H_
