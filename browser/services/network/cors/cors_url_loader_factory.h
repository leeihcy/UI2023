#ifndef SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_
#define SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_

#include <memory>
#include "services/network/url_loader_factory.h"

namespace network {
class NetworkContext;
}

namespace network::cors {
class CorsURLLoaderFactory {
 public:
  CorsURLLoaderFactory(network::NetworkContext* context);
  ~CorsURLLoaderFactory();

  void CreateLoaderAndStart(ResourceRequest& request);

 private:
  // 禁止拷贝
  CorsURLLoaderFactory(const CorsURLLoaderFactory&) = delete;
  CorsURLLoaderFactory& operator=(const CorsURLLoaderFactory&) = delete;

  
  std::unique_ptr<network::URLLoaderFactory> m_network_loader_factory;
};

}  // namespace network::cors

#endif  // SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_
