#ifndef BASE_MEMORY_UNSAFE_SHARED_MEMORY_REGION_H_
#define BASE_MEMORY_UNSAFE_SHARED_MEMORY_REGION_H_

#include "base/memory/platform_shared_memory_region.h"
#include "base/memory/shared_memory_mapping.h"


namespace base {

class UnsafeSharedMemoryRegion {
public:
  UnsafeSharedMemoryRegion() = default;
  // Move operations are allowed.
  UnsafeSharedMemoryRegion(UnsafeSharedMemoryRegion&&) = default;
  UnsafeSharedMemoryRegion& operator=(UnsafeSharedMemoryRegion&&) = default;

  UnsafeSharedMemoryRegion(const UnsafeSharedMemoryRegion&) = delete;
  UnsafeSharedMemoryRegion& operator=(const UnsafeSharedMemoryRegion&) = delete;

  static UnsafeSharedMemoryRegion Deserialize(
      subtle::PlatformSharedMemoryRegion handle) {
    return UnsafeSharedMemoryRegion(std::move(handle));
  }
  UnsafeSharedMemoryRegion Duplicate() const {
    return UnsafeSharedMemoryRegion(handle_.Duplicate());
  }

  bool IsValid() const {
    return handle_.IsValid();
  }

  WritableSharedMemoryMapping Map(SharedMemoryMapper* mapper = nullptr) const;
  WritableSharedMemoryMapping MapAt(uint64_t offset,
                                    size_t size,
                                    SharedMemoryMapper* mapper = nullptr) const;

private:
  explicit UnsafeSharedMemoryRegion(subtle::PlatformSharedMemoryRegion handle)
   : handle_(std::move(handle)) { }

private:
  subtle::PlatformSharedMemoryRegion handle_;
};

}
#endif