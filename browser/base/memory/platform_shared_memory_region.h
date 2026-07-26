#ifndef BASE_MEMORY_PLATFORM_SHARED_MEMORY_REGION_H_
#define BASE_MEMORY_PLATFORM_SHARED_MEMORY_REGION_H_

namespace base {
namespace subtle {

class PlatformSharedMemoryRegion {
public:
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

 private:
  static PlatformSharedMemoryRegion Create(Mode mode, size_t size);


};

}  // namespace subtle
}  // namespace base

#endif