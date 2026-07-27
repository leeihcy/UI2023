#ifndef BASE_MEMORY_PLATFORM_SHARED_MEMORY_HANDLE_H_
#define BASE_MEMORY_PLATFORM_SHARED_MEMORY_HANDLE_H_

#if defined(OS_WIN)
#include "base/win/windows_types.h"
#include "base/win/scoped_handle.h"
#endif

namespace base::subtle {

#if defined(OS_WIN)
using PlatformSharedMemoryHandle = HANDLE;
using ScopedPlatformSharedMemoryHandle = win::ScopedHandle;
#else

#endif

}  // namespace base::subtle

#endif  // BASE_MEMORY_PLATFORM_SHARED_MEMORY_HANDLE_H_
