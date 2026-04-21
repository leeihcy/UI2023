#ifndef SERVICES_NETWORK_URL_LOADER_FACTORY_H_
#define SERVICES_NETWORK_URL_LOADER_FACTORY_H_

#include "services/network/public/mojom/url_loader_factory.mojom.h"

namespace net {
class URLRequestContext;
}

namespace network {
class ResourceRequest;
class NetworkContext;

class URLLoaderContext {
public:
  virtual net::URLRequestContext* GetUrlRequestContext() const = 0;
};

class URLLoaderFactory : public mojom::URLLoaderFactory, public URLLoaderContext {
public:
  URLLoaderFactory(NetworkContext* context);

  void CreateLoaderAndStartWithSyncClient(ResourceRequest& resource_request);
  void CreateLoaderAndStart(ResourceRequest& resource_request) override;

  // URLLoaderContext
  net::URLRequestContext* GetUrlRequestContext() const override;
  
private:
  NetworkContext* m_network_context = nullptr;
};

}

#endif