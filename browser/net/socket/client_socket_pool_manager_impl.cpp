#include "net/socket/client_socket_pool_manager_impl.h"

#include "net/socket/client_socket_pool.h"

namespace net {

ClientSocketPool* ClientSocketPoolManagerImpl::GetSocketPool(/*const ProxyChain& proxy_chain*/) {
  if (!m_pool) {
    m_pool = std::make_unique<TransportClientSocketPool>();
  }
  return m_pool.get();
}

}