#ifndef BASE_WIN_SCOPED_HANDLE_H_
#define BASE_WIN_SCOPED_HANDLE_H_

#include <Windows.h>

namespace base {
namespace win {

template <class Traits>
class GenericScopedHandle {
 public:
  using Handle = typename Traits::Handle;

  GenericScopedHandle() : handle_(Traits::NullHandle()) {}

  explicit GenericScopedHandle(Handle handle) : handle_(Traits::NullHandle()) {
    Set(handle);
  }

  GenericScopedHandle(GenericScopedHandle&& other)
      : handle_(Traits::NullHandle()) {
    Set(other.release());
  }

  GenericScopedHandle(const GenericScopedHandle&) = delete;
  GenericScopedHandle& operator=(const GenericScopedHandle&) = delete;

  ~GenericScopedHandle() { Close(); }

  bool is_valid() const { return Traits::IsHandleValid(handle_); }

  GenericScopedHandle& operator=(GenericScopedHandle&& other) {
    Set(other.release());
    return *this;
  }

  void Set(Handle handle) {
    if (handle_ != handle) {
      // Preserve old LastError to avoid bug 528394.
      auto last_error = ::GetLastError();
      Close();

      if (Traits::IsHandleValid(handle)) {
        handle_ = handle;
      }
      ::SetLastError(last_error);
    }
  }

  Handle get() const { return handle_; }

  // TODO(crbug.com/40212898): Migrate callers to get().
  Handle Get() const { return get(); }

  // Transfers ownership away from this object.
  [[nodiscard]] Handle release() {
    Handle temp = handle_;
    handle_ = Traits::NullHandle();
    return temp;
  }

  // TODO(crbug.com/40212898): Migrate callers to release().
  [[nodiscard]] Handle Take() { return release(); }

  // Explicitly closes the owned handle.
  void Close() {
    if (Traits::IsHandleValid(handle_)) {
      Traits::CloseHandle(handle_);
      handle_ = Traits::NullHandle();
    }
  }

 private:
  Handle handle_;
};


// The traits class for Win32 handles that can be closed via CloseHandle() API.
class HandleTraits {
 public:
  using Handle = HANDLE;

  HandleTraits() = delete;
  HandleTraits(const HandleTraits&) = delete;
  HandleTraits& operator=(const HandleTraits&) = delete;

  // Closes the handle.
  static bool CloseHandle(HANDLE handle) {
    return !!::CloseHandle(handle);
  }

  // Returns true if `handle` is neither null nor a pseudo handle like
  // GetCurrentProcess() or INVALID_HANDLE_VALUE. This means it has a value in
  // the range used for real handle values. It is still possible for `handle` to
  // not be associated with an actual open handle.
  // Note: Use TakeHandleOfType() to adopt a handle of a particular type with
  // additional validation.
  static bool IsHandleValid(HANDLE handle) {
    return handle != nullptr /*&& !base::win::IsPseudoHandle(handle)*/;
  }

  // Returns NULL handle value.
  static HANDLE NullHandle() { return nullptr; }
};

using CheckedScopedHandle = GenericScopedHandle<HandleTraits>;
using ScopedHandle = CheckedScopedHandle;

}
}

#endif