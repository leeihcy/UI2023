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

int TransportClientSocketPool::RequestSocket(const GroupId& group_id, ClientSocketHandle* handle, CompletionOnceCallback callback) {

  std::unique_ptr<Request> request = std::make_unique<Request>(handle, std::move(callback));

  Group *group = nullptr;
  auto group_it = group_map_.find(group_id);
  if (group_it != group_map_.end()) {
    group = group_it->second;

    assert(false); // TODO:
#if 0
    // Try to reuse a socket.
    if (AssignIdleSocketToRequest(request, group))
      return OK;

    // 之前已发起了一个连接，但上层请求被取消了。但连接仍然在进行中，尝试复用。
    if (group->TryToUseNeverAssignedConnectJob())
      return ERR_IO_PENDING;

    // Can we make another active socket now?
    if (!group->HasAvailableSocketSlot(max_sockets_per_group_) &&
        request.respect_limits() == RespectLimits::ENABLED) {
      // 普通请求：等待, 预连接：放弃。
      // 因为Preconnect 本来就是优化没必要排队。
      return preconnecting ? ERR_PRECONNECT_MAX_SOCKET_LIMIT : ERR_IO_PENDING;
    }
#endif
  } 
  // 检查整个 SocketPool 是否达到上限
  // 尝试踢掉 Idle Socket

  // 到这一步则是真有没有可复用的
  group = GetOrCreateGroup(group_id);

  std::unique_ptr<ConnectJob> connect_job(CreateConnectJob(group_id, group));
  int rv = connect_job->Connect();
  if (rv == ERR_IO_PENDING) {
    group->AddJob(std::move(connect_job)/*, preconnecting*/);
    group->InsertUnboundRequest(std::move(request));
  }

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

void TransportClientSocketPool::Group::AddJob(std::unique_ptr<ConnectJob> job/*, bool is_preconnect*/) {
  jobs_.push_back(std::move(job));
}

void TransportClientSocketPool::Group::OnConnectJobComplete(int result,
                                                            ConnectJob* job) {
  assert(ERR_IO_PENDING != result);
  client_socket_pool_->OnConnectJobComplete(this, result, job);
}

void TransportClientSocketPool::Group::InsertUnboundRequest(std::unique_ptr<Request> request) {
  // 这里可以模拟Priority调整各个请求的顺序。
  unbound_requests_.push_back(std::move(request)/*, priority*/);
}
const TransportClientSocketPool::Request* TransportClientSocketPool::Group::GetNextUnboundRequest() const {
  return unbound_requests_.empty() ? nullptr
                                   : unbound_requests_.front().get();
}
std::unique_ptr<TransportClientSocketPool::Request>
TransportClientSocketPool::Group::PopNextUnboundRequest() {
  if (unbound_requests_.empty())
    return nullptr;

  std::unique_ptr<Request> request = std::move(unbound_requests_.front());
  unbound_requests_.pop_front();
  return request;
}

//
// >	net.dll!net::HttpStreamFactory::Job::RunLoop	C++
// >	net.dll!net::HttpStreamFactory::Job::OnIOComplete	C++
//  	net.dll!net::ClientSocketHandle::OnIOComplete	C++
//  	net.dll!net::TransportClientSocketPool::InvokeUserCallback	C++
//
void TransportClientSocketPool::OnConnectJobComplete(Group *group, int result,
                                                     ConnectJob *job) {

  Request *request = nullptr;
  std::unique_ptr<Request> owned_request;
  owned_request = group->PopNextUnboundRequest();
  request = owned_request.get();
  if (!request) {
    // TODO:
    assert(false);
    return;
  }

  InvokeUserCallbackLater(request->handle(), request->release_callback(),
                          result /*, request->socket_tag()*/);
}

void TransportClientSocketPool::InvokeUserCallbackLater(
    ClientSocketHandle *handle, CompletionOnceCallback callback, int result) {
  std::move(callback)(result);
}

}