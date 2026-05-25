#include "net/base/sockaddr_storage.h"

#include <string.h>
#include <winsock2.h>

namespace net {

namespace {

// Templated because otherwise the compiler attempts to instantiate both arms of
// the constexpr if, which causes compile failures in the non-matching arm.
template <typename T>
void copy_from(T& dest, const T& src) {
  // `sockaddr_storage` may contain padding. memcpy treats the object as raw
  // bytes and is safe for any trivially copyable type regardless of padding.
  memcpy(&dest, &src, sizeof(T));
}

}  // namespace

SockaddrStorage::SockaddrStorage() = default;

SockaddrStorage::SockaddrStorage(const SockaddrStorage& other)
    : addr_len(other.addr_len) {
  copy_from(addr_storage, other.addr_storage);
}

SockaddrStorage& SockaddrStorage::operator=(const SockaddrStorage& other) {
  addr_len = other.addr_len;
  copy_from(addr_storage, other.addr_storage);
  return *this;
}

}  // namespace net
