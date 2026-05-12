#include "net/socket/client_socket_handle.h"
#include "net/socket/client_socket_pool.h"

namespace net {

void ClientSocketHandle::Init(const ClientSocketPool::GroupId& group_id, ClientSocketPool *pool) {
  pool->RequestSocket(group_id, this);
}
} // namespace net
