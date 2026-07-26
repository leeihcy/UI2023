#include "base/memory/platform_shared_memory_region.h"

#include <Windows.h>
#include <string>
#include "base/process/process_handle.h"

namespace base::subtle {

// static
PlatformSharedMemoryRegion PlatformSharedMemoryRegion::CreateWritable(
    size_t size) {
  return Create(Mode::kWritable, size);
}

// static
PlatformSharedMemoryRegion PlatformSharedMemoryRegion::CreateUnsafe(
    size_t size) {
  return Create(Mode::kUnsafe, size);
}

#if defined(OS_WIN)

namespace {
// Round up |size| to a multiple of alignment, which must be a power of two.
template <typename T>
[[nodiscard]] inline constexpr T AlignUp(T size, T alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}
inline const wchar_t* as_wcstr(std::u16string_view str) {
  return reinterpret_cast<const wchar_t*>(str.data());
}

HANDLE CreateFileMappingWithReducedPermissions(SECURITY_ATTRIBUTES* sa,
                                               size_t size,
                                               LPCWSTR name) {
  auto create_file_mapping = [&]() {
    return ::CreateFileMapping(INVALID_HANDLE_VALUE, sa, PAGE_READWRITE, 0,
                               static_cast<DWORD>(size), name);
  };

  HANDLE h = create_file_mapping();
  if (!h) {
    return nullptr;
  }

  HANDLE h2;
  ProcessHandle process = GetCurrentProcess();
  BOOL success = ::DuplicateHandle(
      process, h, process, &h2, FILE_MAP_READ | FILE_MAP_WRITE | SECTION_QUERY,
      FALSE, 0);
  const DWORD last_error = ::GetLastError();
  BOOL rv = ::CloseHandle(h);
  if (!success) {
    return nullptr;
  }

  return h2;
}


}

// static
PlatformSharedMemoryRegion PlatformSharedMemoryRegion::Create(Mode mode,
                                                              size_t size) {
  if (size == 0) {
    return {};
  }

  static const size_t kSectionSize = 65536;
  size_t rounded_size = AlignUp(size, kSectionSize);

  // Add an empty DACL to enforce anonymous read-only sections.
  ACL dacl;
  SECURITY_DESCRIPTOR sd;
  if (!InitializeAcl(&dacl, sizeof(dacl), ACL_REVISION)) {
    return {};
  }
  if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
    return {};
  }
  if (!SetSecurityDescriptorDacl(&sd, TRUE, &dacl, FALSE)) {
    return {};
  }

  std::u16string name;
  SECURITY_ATTRIBUTES sa = {sizeof(sa), &sd, FALSE};
  // Ask for the file mapping with reduced permisions to avoid passing the
  // access control permissions granted by default into unpriviledged process.
  HANDLE h = CreateFileMappingWithReducedPermissions(
      &sa, rounded_size, name.empty() ? nullptr : as_wcstr(name));
  if (h == nullptr) {
    return {};
  }

#if 0
  win::ScopedHandle scoped_h(h);
  // Check if the shared memory pre-exists.
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    return {};
  }

  return PlatformSharedMemoryRegion(std::move(scoped_h), mode, size,
                                    UnguessableToken::Create());
#endif
}

#endif

}
