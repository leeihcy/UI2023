#include "net/base/net_export.h"
#include <iostream>

#include "net/socket/tcp_client_socket.h"

namespace net {

void _NetUnitTest() {
  // TCPClientSocket socket;
}

NET_EXPORT void NetUnitTest() {
#if defined(NET_UNITTEST)
  std::cout << "net unit test begin!" << std::endl;
  _NetUnitTest();
  std::cout << "net unit test end!" << std::endl;
#endif
}

}