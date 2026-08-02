#ifndef BASE_MEMORY_PLATFORM_SHARED_MEMORY_REGION_H_
#define BASE_MEMORY_PLATFORM_SHARED_MEMORY_REGION_H_

#include "base/memory/platform_shared_memory_handle.h"
#include "base/memory/shared_memory_mapper.h"
#include <utility>
#include <optional>
#include <span>

namespace base {
namespace subtle {

class PlatformSharedMemoryRegion {
public:
  // Default constructor initializes an invalid instance, i.e. an instance that
  // doesn't wrap any valid platform handle.
  PlatformSharedMemoryRegion() = default;

  // Move operations are allowed.
  PlatformSharedMemoryRegion(PlatformSharedMemoryRegion&&) = default;
  PlatformSharedMemoryRegion& operator=(PlatformSharedMemoryRegion&&) = default;
  PlatformSharedMemoryRegion(const PlatformSharedMemoryRegion&) = delete;
  PlatformSharedMemoryRegion& operator=(const PlatformSharedMemoryRegion&) =
      delete;

  enum class Mode {
    kReadOnly,  // ReadOnlySharedMemoryRegion
    kWritable,  // WritableSharedMemoryRegion
    kUnsafe,    // UnsafeSharedMemoryRegion
    kMaxValue = kUnsafe
  };
  enum class CreateError {
    SUCCESS = 0,
    SIZE_ZERO = 1,
    SIZE_TOO_LARGE = 2,
    INITIALIZE_ACL_FAILURE = 3,
    INITIALIZE_SECURITY_DESC_FAILURE = 4,
    SET_SECURITY_DESC_FAILURE = 5,
    CREATE_FILE_MAPPING_FAILURE = 6,
    REDUCE_PERMISSIONS_FAILURE = 7,
    ALREADY_EXISTS = 8,
    ALLOCATE_FILE_REGION_FAILURE = 9,
    FSTAT_FAILURE = 10,
    INODES_MISMATCH = 11,
    GET_SHMEM_TEMP_DIR_FAILURE = 12,
    kMaxValue = GET_SHMEM_TEMP_DIR_FAILURE
  };
  enum { kMapMinimumAlignment = 32 };

  static PlatformSharedMemoryRegion CreateWritable(size_t size);
  static PlatformSharedMemoryRegion CreateUnsafe(size_t size);

  bool IsValid() const {
    return handle_.is_valid();
  }
  size_t GetSize() const { return size_; }

  ScopedPlatformSharedMemoryHandle PassPlatformHandle() {
    return std::move(handle_);
  }
  static PlatformSharedMemoryRegion Take(
      ScopedPlatformSharedMemoryHandle handle,
      Mode mode,
      size_t size/*,
      const UnguessableToken& guid*/) {
    // return TakeOrFail(std::move(handle), mode, size/*, guid*/).value();
    return PlatformSharedMemoryRegion(std::move(handle), mode, size/*, guid*/);
  }
  PlatformSharedMemoryRegion Duplicate() const;
  PlatformSharedMemoryHandle GetPlatformHandle() const;

  std::optional<std::span<uint8_t>> MapAt(uint64_t offset,
                                     size_t size,
                                     SharedMemoryMapper* mapper) const;

 private:
  static PlatformSharedMemoryRegion Create(Mode mode, size_t size);

  PlatformSharedMemoryRegion(ScopedPlatformSharedMemoryHandle handle,
                             Mode mode,
                             size_t size/*,
                             const UnguessableToken& guid*/);

  ScopedPlatformSharedMemoryHandle handle_;
  Mode mode_ = Mode::kReadOnly;
  size_t size_ = 0;
  // UnguessableToken guid_;
};

}  // namespace subtle
}  // namespace base

#endif