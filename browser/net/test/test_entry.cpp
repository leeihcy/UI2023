#include "net/base/net_export.h"
#include <iostream>

namespace net {

void _NetUnitTest() {
}

NET_EXPORT void NetUnitTest() {
#if defined(NET_UNITTEST)
  std::cout << "net unit test begin!" << std::endl;
  _NetUnitTest();
  std::cout << "net unit test end!" << std::endl;
#endif
}

}