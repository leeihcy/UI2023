#ifndef MOJO_PUBLIC_CPP_SYSTEM_HANDLE_H_
#define MOJO_PUBLIC_CPP_SYSTEM_HANDLE_H_

#include "mojo/public/c/system/types.h"

namespace mojo {

// Scoper for the actual handle types defined further below. It's move-only,
// like the C++11 |unique_ptr|.
template <class HandleType>
class ScopedHandleBase {
 public:
  using RawHandleType = HandleType;

  ScopedHandleBase() {}
  explicit ScopedHandleBase(HandleType handle) : handle_(handle) {}

  ScopedHandleBase(const ScopedHandleBase&) = delete;
  ScopedHandleBase& operator=(const ScopedHandleBase&) = delete;

  ~ScopedHandleBase() { CloseIfNecessary(); }

  template <class CompatibleHandleType>
  explicit ScopedHandleBase(ScopedHandleBase<CompatibleHandleType> other)
      : handle_(other.release()) {}

  // Move-only constructor and operator=.
  ScopedHandleBase(ScopedHandleBase&& other) noexcept
      : handle_(other.release()) {}
  ScopedHandleBase& operator=(ScopedHandleBase&& other) noexcept {
    if (&other != this) {
      CloseIfNecessary();
      handle_ = other.release();
    }
    return *this;
  }

  const HandleType& get() const { return handle_; }
  const HandleType* operator->() const {
    DCHECK(handle_.is_valid());
    return &handle_;
  }

  template <typename PassedHandleType>
  static ScopedHandleBase<HandleType> From(
      ScopedHandleBase<PassedHandleType> other) {
    static_assert(
        sizeof(static_cast<PassedHandleType*>(static_cast<HandleType*>(0))),
        "HandleType is not a subtype of PassedHandleType");
    return ScopedHandleBase<HandleType>(
        static_cast<HandleType>(other.release().value()));
  }

  void swap(ScopedHandleBase& other) { handle_.swap(other.handle_); }

  [[nodiscard]] HandleType release() {
    HandleType rv;
    rv.swap(handle_);
    return rv;
  }

  void reset(HandleType handle = HandleType()) {
    CloseIfNecessary();
    handle_ = handle;
  }

  bool is_valid() const { return handle_.is_valid(); }

  explicit operator bool() const { return handle_.is_valid(); }

  bool operator==(const ScopedHandleBase& other) const {
    return handle_.value() == other.get().value();
  }

 private:
  void CloseIfNecessary() {
    if (handle_.is_valid()) {
      handle_.Close();
    }
  }

  HandleType handle_;
};

template <typename HandleType>
inline ScopedHandleBase<HandleType> MakeScopedHandle(HandleType handle) {
  return ScopedHandleBase<HandleType>(handle);
}
  
// Handle ----------------------------------------------------------------------

const MojoHandle kInvalidHandleValue = MOJO_HANDLE_INVALID;

// Wrapper base class for |MojoHandle|.
class Handle {
 public:
  Handle() : value_(kInvalidHandleValue) {}
  explicit Handle(MojoHandle value) : value_(value) {}
  ~Handle() {}

  void swap(Handle& other) {
    MojoHandle temp = value_;
    value_ = other.value_;
    other.value_ = temp;
  }

  bool is_valid() const { return value_ != kInvalidHandleValue; }

  explicit operator bool() const { return value_ != kInvalidHandleValue; }

  const MojoHandle& value() const { return value_; }
  MojoHandle* mutable_value() { return &value_; }
  void set_value(MojoHandle value) { value_ = value; }

  void Close() {
    // [[maybe_unused]] MojoResult result = MojoClose(value_);
  }

  // HandleSignalsState QuerySignalsState() const {
  //   HandleSignalsState signals_state;
  //   [[maybe_unused]] MojoResult result = MojoQueryHandleSignalsState(
  //       value_, static_cast<MojoHandleSignalsState*>(&signals_state));
  //   DCHECK_EQ(MOJO_RESULT_OK, result);
  //   return signals_state;
  // }

 private:
  MojoHandle value_;

  // Copying and assignment allowed.
};


}

#endif