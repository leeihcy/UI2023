#ifndef SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_
#define SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_

#include <memory>
#include <set>
#include "services/network/url_loader_factory.h"
#include "services/network/url_loader.h"

namespace network {
class NetworkContext;
}

namespace network::cors {
class CorsURLLoaderFactory {
 public:
  CorsURLLoaderFactory(network::NetworkContext* context);
  ~CorsURLLoaderFactory();

  void CreateLoaderAndStart(ResourceRequest& request);

  // Methods for use by network::URLLoaderFactory.
  void OnURLLoaderCreated(std::unique_ptr<URLLoader> loader);

 private:
  // 禁止拷贝
  CorsURLLoaderFactory(const CorsURLLoaderFactory&) = delete;
  CorsURLLoaderFactory& operator=(const CorsURLLoaderFactory&) = delete;

  std::set<std::unique_ptr<URLLoader>> url_loaders_;

  std::unique_ptr<network::URLLoaderFactory> m_network_loader_factory;
};

}  // namespace network::cors

#endif  // SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_
