#include "services/network/cors/cors_url_loader_factory.h"
#include "services/network/cors/cors_url_loader.h"

namespace network::cors {

CorsURLLoaderFactory::CorsURLLoaderFactory(network::NetworkContext* context) {
  m_network_loader_factory = std::make_unique<network::URLLoaderFactory>(context);

}

CorsURLLoaderFactory::~CorsURLLoaderFactory() {
}

void CorsURLLoaderFactory::CreateLoaderAndStart(ResourceRequest& request) {
  auto loader = std::make_unique<CorsURLLoader>(m_network_loader_factory.get());
  loader->SetCorsFlagIfNeeded();
  loader->Start();
}

}  // namespace network::cors
