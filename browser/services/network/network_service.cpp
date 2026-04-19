#include "services/network/network_service.h"
#include "services/network/network_context.h"

namespace network {

NetworkService& NetworkService::GetInstance() {
  static NetworkService s;
  return s;
}

NetworkContext* NetworkService::CreateNetworkContext() {
  std::unique_ptr<NetworkContext> network_context = std::make_unique<NetworkContext>();
  m_network_contexts.push_back(std::move(network_context));

  return m_network_contexts.back().get();
}

}