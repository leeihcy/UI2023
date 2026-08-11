#ifndef BASE_MEMORY_SHARED_MEMORY_MAPPING_H_
#define BASE_MEMORY_SHARED_MEMORY_MAPPING_H_

#include "base/memory/shared_memory_mapper.h"
#include <span>


namespace base {

namespace subtle {
class PlatformSharedMemoryRegion;
} // namespace subtle

class SharedMemoryMapping {
public:
  SharedMemoryMapping();
  virtual ~SharedMemoryMapping();

    // Move operations are allowed.
  SharedMemoryMapping(SharedMemoryMapping&& mapping) noexcept;
  SharedMemoryMapping& operator=(SharedMemoryMapping&& mapping) noexcept;

  SharedMemoryMapping(const SharedMemoryMapping&) = delete;
  SharedMemoryMapping& operator=(const SharedMemoryMapping&) = delete;

  
  bool IsValid() const { return !mapped_span_.empty(); }
  size_t size() const { return size_; }
  size_t mapped_size() const { return mapped_span_.size(); }

protected:
  SharedMemoryMapping(std::span<uint8_t> mapped_span, size_t size,
                      // const UnguessableToken& guid,
                      SharedMemoryMapper *mapper);

  void Unmap();

protected:
  // Returns a span over the full mapped memory.
  std::span<uint8_t> mapped_memory() const { return mapped_span_; }

  std::span<uint8_t> mapped_span_;
  size_t size_ = 0;

  SharedMemoryMapper *mapper_ = nullptr;
};

class ReadOnlySharedMemoryMapping : public SharedMemoryMapping {
public:
  ReadOnlySharedMemoryMapping() = default;
  ReadOnlySharedMemoryMapping(std::span<uint8_t> mapped_span, size_t size,
                              // const UnguessableToken& guid,
                              SharedMemoryMapper *mapper);
};

class WritableSharedMemoryMapping : public SharedMemoryMapping {
public:
  WritableSharedMemoryMapping() = default;
  WritableSharedMemoryMapping(std::span<uint8_t> mapped_span, size_t size,
                              // const UnguessableToken& guid,
                              SharedMemoryMapper *mapper);

  WritableSharedMemoryMapping(const WritableSharedMemoryMapping &) = delete;
  WritableSharedMemoryMapping &
  operator=(const WritableSharedMemoryMapping &) = delete;

    // Move operations are allowed.
  WritableSharedMemoryMapping(WritableSharedMemoryMapping&&) noexcept = default;
  WritableSharedMemoryMapping& operator=(
      WritableSharedMemoryMapping&&) noexcept = default;

  uint8_t *data() { return mapped_memory().data(); }
  const uint8_t *data() const { return mapped_memory().data(); }

  void *memory() { return data(); }
  const void *memory() const { return data(); }
};

} // namespace base

#endif