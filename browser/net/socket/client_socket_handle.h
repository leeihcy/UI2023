#ifndef NET_SOCKET_CLIENT_SOCKET_HANDLE_H_
#define NET_SOCKET_CLIENT_SOCKET_HANDLE_H_

#include "net/socket/stream_socket_handle.h"
#include "net/socket/client_socket_pool.h"

namespace net {
class ClientSocketPool;

class ClientSocketHandle : public StreamSocketHandle {
public:
  void Init(const ClientSocketPool::GroupId& group_id, ClientSocketPool* pool);
};

}

#endif