#ifndef BASE_MEMORY_WRITABLE_SHARED_MEMORY_REGION_H_
#define BASE_MEMORY_WRITABLE_SHARED_MEMORY_REGION_H_

#include <memory>
#include "base/memory/platform_shared_memory_region.h"

namespace base {

class WritableSharedMemoryRegion {
public:
  static WritableSharedMemoryRegion Create(size_t size);
  explicit WritableSharedMemoryRegion(
      subtle::PlatformSharedMemoryRegion handle);

  static subtle::PlatformSharedMemoryRegion TakeHandleForSerialization(
      WritableSharedMemoryRegion region);

private:
  subtle::PlatformSharedMemoryRegion handle_;
};

}

#endif