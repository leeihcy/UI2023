
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

  virtual int RequestSocket(ClientSocketHandle *handle) = 0;

protected:
  const std::unique_ptr<ConnectJobFactory> m_connect_job_factory;
};

// 另一个pool是WebSocketTransportClientSocketPool
class TransportClientSocketPool : public ClientSocketPool {
public:
  TransportClientSocketPool();

  int RequestSocket(ClientSocketHandle* handle) override;
  std::unique_ptr<ConnectJob> CreateConnectJob();

private:
};

}

#endif