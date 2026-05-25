#ifndef NET_BASE_WINSOCK_INIT_H_
#define NET_BASE_WINSOCK_INIT_H_

#include "net/base/net_export.h"

namespace net {

// Make sure that Winsock is initialized, calling WSAStartup if needed.
void EnsureWinsockInit();

}  // namespace net

#endif  // NET_BASE_WINSOCK_INIT_H_
