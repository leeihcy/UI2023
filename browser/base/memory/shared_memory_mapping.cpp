
#include "base/memory/shared_memory_mapping.h"

#include <cstdint>
#include <utility>

namespace base {

SharedMemoryMapping::SharedMemoryMapping() = default;

SharedMemoryMapping::SharedMemoryMapping(SharedMemoryMapping&& mapping) noexcept
    : mapped_span_(std::exchange(mapping.mapped_span_, std::span<uint8_t>())),
      size_(mapping.size_),
      // guid_(mapping.guid_),
      mapper_(mapping.mapper_) {}

SharedMemoryMapping::~SharedMemoryMapping() {
  Unmap();
}

SharedMemoryMapping::SharedMemoryMapping(std::span<uint8_t> mapped_span,
                                         size_t size,
                                        //  const UnguessableToken& guid,
                                         SharedMemoryMapper* mapper)
    : mapped_span_(mapped_span), size_(size) /*, guid_(guid)*/, mapper_(mapper) {
  // Note: except on Windows, `mapped_span_.size() == size_`.
  // SharedMemoryTracker::GetInstance()->IncrementMemoryUsage(*this);
}

void SharedMemoryMapping::Unmap() {
  if (!IsValid()) {
    return;
  }

  // SharedMemorySecurityPolicy::ReleaseReservationForMapping(size_);
  // SharedMemoryTracker::GetInstance()->DecrementMemoryUsage(*this);

  SharedMemoryMapper* mapper = mapper_;
  if (!mapper) {
    mapper = SharedMemoryMapper::GetDefaultInstance();
  }

  // The backing mapper expects offset to be aligned to
  // `SysInfo::VMAllocationGranularity()`, so replicate the alignment that was
  // done when originally mapping in the region.
  uint8_t* aligned_data = mapped_span_.data();
  size_t adjusted_size =
      mapped_span_.size() +
      static_cast<size_t>(mapped_span_.data() - aligned_data);
  mapper->Unmap(std::span(aligned_data, adjusted_size));
}


ReadOnlySharedMemoryMapping::ReadOnlySharedMemoryMapping(
    std::span<uint8_t> mapped_span,
    size_t size,
    // const UnguessableToken& guid,
    SharedMemoryMapper* mapper)
    : SharedMemoryMapping(mapped_span, size/*, guid*/, mapper) {}

WritableSharedMemoryMapping::WritableSharedMemoryMapping(
    std::span<uint8_t> mapped_span,
    size_t size,
    // const UnguessableToken& guid,
    SharedMemoryMapper* mapper)
    : SharedMemoryMapping(mapped_span, size/*, guid*/, mapper) {}

}