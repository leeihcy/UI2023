#ifndef BASE_MEMORY_UNSAFE_SHARED_MEMORY_REGION_H_
#define BASE_MEMORY_UNSAFE_SHARED_MEMORY_REGION_H_

#include "base/memory/platform_shared_memory_region.h"

namespace base {

class UnsafeSharedMemoryRegion {
public:
  static UnsafeSharedMemoryRegion Deserialize(
      subtle::PlatformSharedMemoryRegion handle) {
    return UnsafeSharedMemoryRegion(std::move(handle));
  }
  UnsafeSharedMemoryRegion Duplicate() const {
    return UnsafeSharedMemoryRegion(handle_.Duplicate());
  }

private:
  explicit UnsafeSharedMemoryRegion(subtle::PlatformSharedMemoryRegion handle)
   : handle_(std::move(handle)) { }

private:
  subtle::PlatformSharedMemoryRegion handle_;
};

}
#endif