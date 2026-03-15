#include "net/base/net_export.h"

namespace net {
NET_EXPORT void NetUnitTest();
}

int main() {
#if defined(_DEBUG)
  net::NetUnitTest();
#endif
  return 0;
}