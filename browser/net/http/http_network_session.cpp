#include "net/http/http_network_session.h"

namespace net {

HttpNetworkSession::HttpNetworkSession() {
  m_http_stream_factory = std::make_unique<HttpStreamFactory>(this);

  m_normal_socket_pool_manager = std::make_unique<ClientSocketPoolManagerImpl>();
}

ClientSocketPool *HttpNetworkSession::GetSocketPool() {
  return m_normal_socket_pool_manager->GetSocketPool(/*proxy_chain*/);
}

}