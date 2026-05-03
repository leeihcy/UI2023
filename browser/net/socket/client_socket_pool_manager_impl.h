#ifndef NET_SOCKET_CLIENT_SOCKET_POOL_MANAGER_IMPL_H_
#define NET_SOCKET_CLIENT_SOCKET_POOL_MANAGER_IMPL_H_

#include <memory>

namespace net {
class ClientSocketPool;

class ClientSocketPoolManager {
public:
  virtual ~ClientSocketPoolManager() {}

  // Returns the socket pool for the specified ProxyChain (Which may be
  // ProxyChain::Direct()).
  virtual ClientSocketPool* GetSocketPool(/*const ProxyChain& proxy_chain*/) = 0;

};


class ClientSocketPoolManagerImpl : public ClientSocketPoolManager {
public:
  ClientSocketPool* GetSocketPool(/*const ProxyChain& proxy_chain*/) override;

  // 简单处理，只返回一个。TransportClientSocketPool
  std::unique_ptr<ClientSocketPool> m_pool = nullptr;

  // std::map<ProxyChain, std::unique_ptr<ClientSocketPool>> socket_pools_;;
};


}
#endif