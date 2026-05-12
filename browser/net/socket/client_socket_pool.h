
#ifndef NET_SOCKET_CLIENT_SOCKET_POOL_H_
#define NET_SOCKET_CLIENT_SOCKET_POOL_H_

#include "net/socket/connect_job.h"
#include "net/socket/connect_job_factory.h"
#include <memory>

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
   const url::SchemeHostPort& destination() const { return destination_; }

    // The endpoint of the final destination (not the proxy).
    url::SchemeHostPort destination_;
  };

  virtual int RequestSocket(const GroupId& group_id, ClientSocketHandle *handle) = 0;

  std::unique_ptr<ConnectJob> CreateConnectJob(GroupId group_id);
protected:
  const std::unique_ptr<ConnectJobFactory> m_connect_job_factory;
};

// 另一个pool是WebSocketTransportClientSocketPool
class TransportClientSocketPool : public ClientSocketPool {
public:
  TransportClientSocketPool();

  int RequestSocket(const GroupId& group_id, ClientSocketHandle* handle) override;


private:
};

}

#endif