#include "net/socket/client_socket_pool.h"

namespace net {

TransportClientSocketPool::TransportClientSocketPool() : ClientSocketPool(std::make_unique<ConnectJobFactory>()) {

}

int TransportClientSocketPool::RequestSocket(const GroupId& group_id, ClientSocketHandle* handle) {
  std::unique_ptr<ConnectJob> connect_job(CreateConnectJob(group_id));
  int rv = connect_job->Connect();
  return rv;
}

std::unique_ptr<ConnectJob> ClientSocketPool::CreateConnectJob(GroupId group_id) {
  return m_connect_job_factory->CreateConnectJob(group_id.destination());
}

}