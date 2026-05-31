#include "net/socket/client_socket_handle.h"
#include "net/socket/client_socket_pool.h"

namespace net {

void ClientSocketHandle::Init(const ClientSocketPool::GroupId& group_id, ClientSocketPool *pool) {
  auto callback = std::bind(&ClientSocketHandle::OnIOComplete, this, std::placeholders::_1);
  pool->RequestSocket(group_id, this, callback);
}

void ClientSocketHandle::OnIOComplete(int result) {

}

} // namespace net
