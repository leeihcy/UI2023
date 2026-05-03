#include "net/socket/client_socket_pool.h"

namespace net {

int TransportClientSocketPool::RequestSocket(ClientSocketHandle* handle) {
  // std::unique_ptr<ConnectJob> connect_job(CreateConnectJob());

  // int rv = connect_job->Connect();
  return -1; //TODO:
}

}