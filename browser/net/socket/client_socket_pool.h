
#ifndef NET_SOCKET_CLIENT_SOCKET_POOL_H_
#define NET_SOCKET_CLIENT_SOCKET_POOL_H_

#include "net/socket/connect_job.h"
#include "net/socket/connect_job_factory.h"
#include <memory>
#include <map>

namespace net {
class ClientSocketHandle;

class ClientSocketPool /* : public LowerLayeredPool */ {
public:
  // Indicates whether or not a request for a socket should respect the
  // SocketPool's global and per-group socket limits.
  enum class RespectLimits { DISABLED, ENABLED };
  
  ClientSocketPool(std::unique_ptr<ConnectJobFactory> connect_job_factory)
      : m_connect_job_factory(std::move(connect_job_factory)) {}

  // Group ID for a socket request. Requests with the same group ID are
  // considered indistinguishable.
  class GroupId {
  public:
    const url::SchemeHostPort &destination() const { return destination_; }
    bool operator<(const GroupId &other) const {
      return destination_ < other.destination_;
    }

    // The endpoint of the final destination (not the proxy).
    url::SchemeHostPort destination_;
  };

  virtual int RequestSocket(const GroupId& group_id, ClientSocketHandle *handle, CompletionOnceCallback callback) = 0;

  std::unique_ptr<ConnectJob> CreateConnectJob(GroupId group_id, ConnectJob::Delegate* delegate);
protected:
  const std::unique_ptr<ConnectJobFactory> m_connect_job_factory;
};

// 另一个pool是WebSocketTransportClientSocketPool
class TransportClientSocketPool : public ClientSocketPool {
public:
  TransportClientSocketPool();

  // 注意：Request 和 ConnectJob 并不存在固定的一对一绑定关系。
  //
  // 当需要创建新连接时：
  // 1. 创建 Request，并放入 Group::unbound_requests_。
  // 2. 创建 ConnectJob，并放入 Group::jobs_。
  // 3. Request 与 ConnectJob 在创建阶段不会直接关联。
  //
  // 当 ConnectJob 建连完成后：
  // 1. ConnectJob 生成可用的 Socket。
  // 2. Group 从 unbound_requests_ 中选择一个待处理 Request。
  // 3. 将 Socket 交付给该 Request，并执行其回调。
  //
  // 这种设计使 Request 与 ConnectJob 解耦，能够支持：
  // - Request 在连接建立期间被取消；
  // - Request 优先级动态调整；
  // - ConnectJob 完成后再决定将连接分配给哪个 Request；
  // - Group 统一管理连接资源和请求调度。
  //
  class Request {
  public:
    Request(ClientSocketHandle *handle, CompletionOnceCallback callback)
        : handle_(handle), callback_(callback) {}
    ClientSocketHandle *handle() const { return handle_; }
    CompletionOnceCallback release_callback() { return std::move(callback_); }

  private:
    ClientSocketHandle *handle_;
    CompletionOnceCallback callback_;
  };

  // Group是核心调度者。某个目标 Host:Port 的连接池实例
  // Group最重要的职责
  // 1. 控制每个Host最大连接数 (max_sockets_per_group)
  // 2. 连接复用
  // 3. 启动新的ConnectJob
  // ConnectJob建立一个连接, Group负责管理一组连接,关系类似：
  // Group
  //    │
  //    ├── ConnectJob1
  //    ├── ConnectJob2
  //    ├── ConnectJob3
  //    └── IdleSockets
  // 
  class Group : public ConnectJob::Delegate {
  public:
    Group(const GroupId& group_id,
          TransportClientSocketPool* client_socket_pool);
    ~Group() override;

    void AddJob(std::unique_ptr<ConnectJob> job/*, bool is_preconnect*/);

    // ConnectJob::Delegate methods:
    void OnConnectJobComplete(int result, ConnectJob *job) override;

    void InsertUnboundRequest(std::unique_ptr<Request> request);
    const Request* GetNextUnboundRequest() const;
    std::unique_ptr<Request> PopNextUnboundRequest();

  private:
    TransportClientSocketPool* client_socket_pool_;
    const GroupId group_id_;

    // socket connect list
    using JobList = std::list<std::unique_ptr<ConnectJob>>;
    JobList jobs_; 

    // 请求开始时，并不会绑定到一个ConnectJob
    using RequestQueue = std::list<std::unique_ptr<Request>>;
    RequestQueue unbound_requests_;
  };

  int RequestSocket(const GroupId& group_id, ClientSocketHandle* handle, CompletionOnceCallback callback) override;
  Group* GetOrCreateGroup(const GroupId& group_id);

public:
  // These correspond to ConnectJob::Delegate methods, and are invoked by the
  // Group a ConnectJob belongs to.
  void OnConnectJobComplete(Group *group, int result, ConnectJob *job);
  void InvokeUserCallbackLater(ClientSocketHandle *handle,
                               CompletionOnceCallback callback, int rv);

                                 // Assigns |socket| to |handle| and updates |group|'s counters appropriately.
  void HandOutSocket(std::unique_ptr<StreamSocket> socket, ClientSocketHandle* handle);

private:
  using GroupMap = std::map<GroupId, Group *>;
  GroupMap group_map_;

};
}

#endif