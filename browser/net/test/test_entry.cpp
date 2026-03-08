#include "net/include/net_api.h"
#include <iostream>

namespace net {

void _NetUnitTest() {
}

NETAPI void NetUnitTest() {
#if defined(NET_UNITTEST)
  std::cout << "net unit test begin!" << std::endl;
  _NetUnitTest();
  std::cout << "net unit test end!" << std::endl;
#endif
}

}