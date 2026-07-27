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

// static
subtle::PlatformSharedMemoryRegion
WritableSharedMemoryRegion::TakeHandleForSerialization(
    WritableSharedMemoryRegion region) {
  return std::move(region.handle_);
}

}