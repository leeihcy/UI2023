#include "base/memory/writable_shared_memory_region.h"

namespace base {

// static 
WritableSharedMemoryRegion WritableSharedMemoryRegion::Create(size_t size) {
    subtle::PlatformSharedMemoryRegion handle =
      subtle::PlatformSharedMemoryRegion::CreateWritable(size);
  return WritableSharedMemoryRegion(std::move(handle));
}


WritableSharedMemoryRegion::WritableSharedMemoryRegion(
    subtle::PlatformSharedMemoryRegion handle)
    : handle_(std::move(handle)) {
}

}