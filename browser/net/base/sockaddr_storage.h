#ifndef NET_BASE_SOCKADDR_STORAGE_H_
#define NET_BASE_SOCKADDR_STORAGE_H_

#include <winsock2.h>
#include <ws2tcpip.h>

namespace net {

// Convenience struct for when you need a |struct sockaddr|.
struct SockaddrStorage {
  SockaddrStorage();
  SockaddrStorage(const SockaddrStorage& other);
  SockaddrStorage& operator=(const SockaddrStorage& other);

  const sockaddr* addr() const {
    return reinterpret_cast<const sockaddr*>(&addr_storage);
  }
  sockaddr* addr() { return reinterpret_cast<sockaddr*>(&addr_storage); }

  sockaddr_storage addr_storage;
  socklen_t addr_len = sizeof(addr_storage);
};

}  // namespace net

#endif  // NET_BASE_SOCKADDR_STORAGE_H_
