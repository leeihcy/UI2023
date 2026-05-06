#include "net/socket/client_socket_pool.h"

namespace net {

TransportClientSocketPool::TransportClientSocketPool() : ClientSocketPool(std::make_unique<ConnectJobFactory>()) {

}

int TransportClientSocketPool::RequestSocket(ClientSocketHandle* handle) {
  std::unique_ptr<ConnectJob> connect_job(CreateConnectJob());
  int rv = connect_job->Connect();
  return rv;
}

std::unique_ptr<ConnectJob> TransportClientSocketPool::CreateConnectJob() {
  return m_connect_job_factory->CreateConnectJob();
}

}