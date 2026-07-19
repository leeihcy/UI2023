#include "services/network/cors/cors_url_loader_factory.h"
#include "services/network/cors/cors_url_loader.h"

#include <assert.h>

namespace network::cors {

CorsURLLoaderFactory::CorsURLLoaderFactory(network::NetworkContext* context) {
  m_network_loader_factory = std::make_unique<network::URLLoaderFactory>(context, this);

}

CorsURLLoaderFactory::~CorsURLLoaderFactory() {
}

void CorsURLLoaderFactory::CreateLoaderAndStart(ResourceRequest& request, mojom::URLLoaderClient* client) {
  auto loader = std::make_unique<CorsURLLoader>(request, client, m_network_loader_factory.get());
  loader->SetCorsFlagIfNeeded();

  auto* raw_loader = loader.get();
  OnCorsURLLoaderCreated(std::move(loader));
  raw_loader->Start();
}

void CorsURLLoaderFactory::OnCorsURLLoaderCreated(
    std::unique_ptr<CorsURLLoader> loader) {
  // OnLoaderCreated(std::move(loader), cors_url_loaders_);
  cors_url_loaders_.insert(std::move(loader));
}


void CorsURLLoaderFactory::OnURLLoaderCreated(
    std::unique_ptr<URLLoader> loader) {
  // OnLoaderCreated(std::move(loader), url_loaders_);
    //   context_->LoaderCreated(process_id_);
    // loaders.insert(std::move(loader));
    url_loaders_.insert(std::move(loader));
}

void CorsURLLoaderFactory::DestroyLoader(URLLoader *loader) {
  auto it = url_loaders_.begin();
  for (; it != url_loaders_.end(); ++it) {
    if (it->get() == loader) {
      url_loaders_.erase(it);
      break;
    }
  }

   assert(false);
}

void CorsURLLoaderFactory::DestroyLoader(CorsURLLoader* loader) {
  auto it = cors_url_loaders_.begin();
  for (; it != cors_url_loaders_.end(); ++it) {
    if (it->get() == loader) {
      cors_url_loaders_.erase(it);
      break;
    }
  }

  assert(false);
}

}  // namespace network::cors
