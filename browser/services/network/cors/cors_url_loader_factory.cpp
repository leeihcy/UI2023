#include "services/network/cors/cors_url_loader_factory.h"
#include "services/network/cors/cors_url_loader.h"

namespace network::cors {

CorsURLLoaderFactory::CorsURLLoaderFactory() {
}

CorsURLLoaderFactory::~CorsURLLoaderFactory() {
}

void CorsURLLoaderFactory::CreateLoaderAndStart() {
  auto loader = std::make_unique<CorsURLLoader>();
  loader->SetCorsFlagIfNeeded();
  loader->Start();
}

}  // namespace network::cors
