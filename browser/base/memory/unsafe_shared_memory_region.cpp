#include "base/memory/unsafe_shared_memory_region.h"

namespace base {

WritableSharedMemoryMapping UnsafeSharedMemoryRegion::Map(
    SharedMemoryMapper* mapper) const {
  return MapAt(0, handle_.GetSize(), mapper);
}

WritableSharedMemoryMapping UnsafeSharedMemoryRegion::MapAt(
    uint64_t offset,
    size_t size,
    SharedMemoryMapper* mapper) const {
  if (!IsValid()) {
    return {};
  }

  auto result = handle_.MapAt(offset, size, mapper);
  if (!result.has_value()) {
    return {};
  }

  return WritableSharedMemoryMapping(result.value(), size/*, handle_.GetGUID()*/,
                                     mapper);
}
}