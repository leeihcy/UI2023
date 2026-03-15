#ifndef NET_SOCKET_TCP_SOCKET_WIN_H_
#define NET_SOCKET_TCP_SOCKET_WIN_H_

#include <winsock2.h>

#include <stdint.h>

#include <memory>

#include "net/base/address_family.h"


namespace net {
class TCPSocketWin {
public:
  int Open(AddressFamily family);

  bool IsValid() const { return socket_ != INVALID_SOCKET; }

private:
  SOCKET socket_;
};

}

#endif