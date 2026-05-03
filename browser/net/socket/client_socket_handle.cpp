#include "net/socket/client_socket_handle.h"
#include "net/socket/client_socket_pool.h"

namespace net {

void ClientSocketHandle::Init(ClientSocketPool *pool) {
  pool->RequestSocket(this);
}
} // namespace net
