
#ifndef NET_SOCKET_CLIENT_SOCKET_POOL_H_
#define NET_SOCKET_CLIENT_SOCKET_POOL_H_

namespace net {
class ClientSocketHandle;

class ClientSocketPool /* : public LowerLayeredPool */ {
public:
  virtual int RequestSocket(ClientSocketHandle* handle) = 0;
};

class TransportClientSocketPool : public ClientSocketPool {
public:
  int RequestSocket(ClientSocketHandle* handle) override;
};

}

#endif