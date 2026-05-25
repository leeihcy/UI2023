#include "net/socket/client_socket_pool.h"

namespace net {

TransportClientSocketPool::TransportClientSocketPool() : ClientSocketPool(std::make_unique<ConnectJobFactory>()) {

}

TransportClientSocketPool::Group* TransportClientSocketPool::GetOrCreateGroup(
    const GroupId& group_id) {
  auto it = group_map_.find(group_id);
  if (it != group_map_.end()) {
    return it->second;
  }
  Group* group = new Group(group_id, this);
  group_map_[group_id] = group;
  return group;
}

int TransportClientSocketPool::RequestSocket(const GroupId& group_id, ClientSocketHandle* handle) {

  Group* group = nullptr;
  group = GetOrCreateGroup(group_id);

  std::unique_ptr<ConnectJob> connect_job(CreateConnectJob(group_id, group));
  int rv = connect_job->Connect();
  return rv;
}

std::unique_ptr<ConnectJob> ClientSocketPool::CreateConnectJob(GroupId group_id, ConnectJob::Delegate* delegate) {
  return m_connect_job_factory->CreateConnectJob(group_id.destination(), delegate);
}

TransportClientSocketPool::Group::Group(
    const GroupId& group_id,
    TransportClientSocketPool* client_socket_pool)
    : group_id_(group_id),
      client_socket_pool_(client_socket_pool) {}

TransportClientSocketPool::Group::~Group() {
}
void TransportClientSocketPool::Group::OnConnectJobComplete(int result,
                                                            ConnectJob* job) {
  assert(ERR_IO_PENDING != result);
  client_socket_pool_->OnConnectJobComplete(this, result, job);
}

void TransportClientSocketPool::OnConnectJobComplete(Group *group, int result,
                                                     ConnectJob *job) {}
}