#ifndef NET_SOCKET_TCP_SOCKET_WIN_H_
#define NET_SOCKET_TCP_SOCKET_WIN_H_

#include <stdint.h>

#include <memory>

#include "net/base/address_family.h"

namespace net {
class TCPSocketPosix {
public:
  int Open(AddressFamily family);

  bool IsValid() const { return false; }

private:
};

}

#endif