#ifndef SERVICES_NETWORK_URL_LOADER_FACTORY_H_
#define SERVICES_NETWORK_URL_LOADER_FACTORY_H_

#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_loader.mojom.h"

namespace net {
class URLRequestContext;
}

namespace network {
class ResourceRequest;
class NetworkContext;
namespace cors {
class CorsURLLoaderFactory;
}  // namespace cors


class URLLoaderContext {
public:
  virtual net::URLRequestContext* GetUrlRequestContext() const = 0;
};

class URLLoaderFactory : public mojom::URLLoaderFactory, public URLLoaderContext {
public:
  URLLoaderFactory(NetworkContext* context, cors::CorsURLLoaderFactory *cors_url_loader_factory);

  void CreateLoaderAndStartWithSyncClient(ResourceRequest &resource_request,
                                          mojom::URLLoaderClient *client);
  void CreateLoaderAndStart(ResourceRequest &resource_request,
                            mojom::URLLoaderClient *client) override;

  // URLLoaderContext
  net::URLRequestContext* GetUrlRequestContext() const override;
  
private:
  NetworkContext* m_network_context = nullptr;

  // |cors_url_loader_factory_| owns this.
  cors::CorsURLLoaderFactory* const cors_url_loader_factory_;
};

}

#endif