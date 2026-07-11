#include "services/network/cors/cors_url_loader_factory.h"
#include "services/network/cors/cors_url_loader.h"

namespace network::cors {

CorsURLLoaderFactory::CorsURLLoaderFactory(network::NetworkContext* context) {
  m_network_loader_factory = std::make_unique<network::URLLoaderFactory>(context, this);

}

CorsURLLoaderFactory::~CorsURLLoaderFactory() {
}

void CorsURLLoaderFactory::CreateLoaderAndStart(ResourceRequest& request) {
  auto loader = std::make_unique<CorsURLLoader>(request, m_network_loader_factory.get());
  loader->SetCorsFlagIfNeeded();
  loader->Start();
}

void CorsURLLoaderFactory::OnURLLoaderCreated(
    std::unique_ptr<URLLoader> loader) {
  // OnLoaderCreated(std::move(loader), url_loaders_);
    //   context_->LoaderCreated(process_id_);
    // loaders.insert(std::move(loader));
    url_loaders_.insert(std::move(loader));
}

}  // namespace network::cors
