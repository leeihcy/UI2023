
#include "base/memory/platform_shared_memory_region.h"

namespace base::subtle {

HANDLE PlatformSharedMemoryRegion::GetPlatformHandle() const {
  return handle_.get();
}

// bool PlatformSharedMemoryRegion::IsValid() const {
//   return handle_.is_valid();
// }


}