#include "net/socket/client_socket_handle.h"
#include "net/socket/client_socket_pool.h"

namespace net {

int ClientSocketHandle::Init(const ClientSocketPool::GroupId& group_id, CompletionOnceCallback callback, ClientSocketPool *pool) {
  auto io_complete_callback = std::bind(&ClientSocketHandle::OnIOComplete, this, std::placeholders::_1);
  int rv = pool->RequestSocket(group_id, this, io_complete_callback);

  if (rv == ERR_IO_PENDING) {
    callback_ = std::move(callback);
  } else {
    // HandleInitCompletion(rv);
  }
  return rv;
}

void ClientSocketHandle::OnIOComplete(int result) {
  CompletionOnceCallback callback = std::move(callback_);
  // HandleInitCompletion(result);
  std::move(callback)(result);
}

} // namespace net
