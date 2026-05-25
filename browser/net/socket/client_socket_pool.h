
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

  virtual int RequestSocket(const GroupId& group_id, ClientSocketHandle *handle) = 0;

  std::unique_ptr<ConnectJob> CreateConnectJob(GroupId group_id, ConnectJob::Delegate* delegate);
protected:
  const std::unique_ptr<ConnectJobFactory> m_connect_job_factory;
};

// 另一个pool是WebSocketTransportClientSocketPool
class TransportClientSocketPool : public ClientSocketPool {
public:
  TransportClientSocketPool();

  class Group : public ConnectJob::Delegate {
  public:
    Group(const GroupId& group_id,
          TransportClientSocketPool* client_socket_pool);
    ~Group() override;

    // ConnectJob::Delegate methods:
    void OnConnectJobComplete(int result, ConnectJob *job) override;
  private:
    const GroupId group_id_;
    TransportClientSocketPool* client_socket_pool_;
  };

  int RequestSocket(const GroupId& group_id, ClientSocketHandle* handle) override;
  Group* GetOrCreateGroup(const GroupId& group_id);

public:
  // These correspond to ConnectJob::Delegate methods, and are invoked by the
  // Group a ConnectJob belongs to.
  void OnConnectJobComplete(Group *group, int result, ConnectJob *job);

private:
  using GroupMap = std::map<GroupId, Group *>;
  GroupMap group_map_;

};
}

#endif